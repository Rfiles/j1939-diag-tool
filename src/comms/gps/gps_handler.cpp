/**
 * J1939 Diagnostic Tool - GPS Handler Task Implementation
 * 
 * Versão: 2.3.0
 */

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "gps_handler.h"

// --- RTOS Objects ---
QueueHandle_t gps_data_queue;
static SemaphoreHandle_t gps_data_mutex;

// --- Module-level state ---
static GpsData last_gps_data;

// --- NMEA Parser (from scratch) ---
#define NMEA_BUFFER_SIZE 100
char nmea_buffer[NMEA_BUFFER_SIZE];
uint8_t nmea_idx = 0;

// Simple helper to parse a float from a comma-separated string
float parse_nmea_float(char** context) {
    char* token = strtok_r(NULL, ",", context);
    return token ? atof(token) : 0.0f;
}

// Parses a GPRMC sentence
void parse_gprmc(char* sentence) {
    GpsData data;
    char* context;

    strtok_r(sentence, ",", &context); // Sentence type ($GPRMC)
    parse_nmea_float(&context); // UTC time

    char* validity = strtok_r(NULL, ",", &context);
    if (validity && validity[0] == 'A') {
        data.is_valid = true;
    } else {
        data.is_valid = false;
        xQueueSend(gps_data_queue, &data, 0);
        return; // No need to parse rest if data is invalid
    }

    float lat = parse_nmea_float(&context);
    char* ns = strtok_r(NULL, ",", &context);
    float lon = parse_nmea_float(&context);
    char* ew = strtok_r(NULL, ",", &context);
    float speed_knots = parse_nmea_float(&context);

    // Convert from NMEA format (DDMM.MMMM) to Decimal Degrees
    data.latitude = floor(lat / 100) + fmod(lat, 100) / 60.0;
    if (ns && ns[0] == 'S') data.latitude = -data.latitude;

    data.longitude = floor(lon / 100) + fmod(lon, 100) / 60.0;
    if (ew && ew[0] == 'W') data.longitude = -data.longitude;

    // Convert speed from knots to km/h
    data.speed_kmh = speed_knots * 1.852;

    // Update shared data with mutex protection
    if (xSemaphoreTake(gps_data_mutex, portMAX_DELAY) == pdTRUE) {
        last_gps_data = data;
        xSemaphoreGive(gps_data_mutex);
    }

    // Send the parsed data to the queue for other real-time tasks
    xQueueSend(gps_data_queue, &data, pdMS_TO_TICKS(10));
}

// --- RTOS Task ---
void gps_task_fn(void* pv) {
    // Assuming GPS is connected to Serial1 (pins GPIO18, GPIO17 on many ESP32-S3)
    Serial1.begin(9600);

    for (;;) {
        if (Serial1.available()) {
            char c = Serial1.read();
            if (c == '$') {
                nmea_idx = 0;
            }
            if (nmea_idx < NMEA_BUFFER_SIZE - 1) {
                nmea_buffer[nmea_idx++] = c;
            }
            if (c == '\n') {
                nmea_buffer[nmea_idx] = '\0';
                if (strncmp(nmea_buffer, "$GPRMC", 6) == 0) {
                    parse_gprmc(nmea_buffer);
                }
                nmea_idx = 0;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

// --- Public API ---
void gps_handler_init() {
    gps_data_queue = xQueueCreate(5, sizeof(GpsData));
    gps_data_mutex = xSemaphoreCreateMutex();
    xTaskCreate(gps_task_fn, "GPS Task", 4096, NULL, 3, NULL);
}

bool gps_get_last_data(GpsData* data) {
    if (data == NULL) {
        return false;
    }
    if (xSemaphoreTake(gps_data_mutex, portMAX_DELAY) == pdTRUE) {
        *data = last_gps_data;
        xSemaphoreGive(gps_data_mutex);
        return data->is_valid;
    }
    return false;
}
