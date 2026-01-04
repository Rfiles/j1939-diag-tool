/**
 * J1939 Diagnostic Tool - GPS Handler Task Implementation
 * 
 * Versão: 2.7.0
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

// Helper to parse an integer from a comma-separated string
int parse_nmea_int(char** context) {
    char* token = strtok_r(NULL, ",", context);
    return token ? atoi(token) : 0;
}

// Helper to parse time from GPRMC sentences (hhmmss.sss)
void parse_nmea_time(char* token, GpsData& data) {
    if (token) {
        int time = atoi(token);
        data.hour = time / 10000;
        data.minute = (time / 100) % 100;
        data.second = time % 100;
    }
}

// Helper to parse date from GPRMC sentences (ddmmyy)
void parse_nmea_date(char* token, GpsData& data) {
    if (token) {
        int date = atoi(token);
        data.day = date / 10000;
        data.month = (date / 100) % 100;
        data.year = 2000 + (date % 100);
    }
}

// Parses a GSA sentence for fix status
void parse_gsa(char* sentence) {
    char* context;
    strtok_r(sentence, ",", &context); // Sentence type ($GPGSA)
    strtok_r(NULL, ",", &context); // Mode 1
    int fix_type = parse_nmea_int(&context);

    if (xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        switch (fix_type) {
            case 2:
                last_gps_data.fix_status = GPS_FIX_2D;
                break;
            case 3:
                last_gps_data.fix_status = GPS_FIX_3D;
                break;
            default:
                last_gps_data.fix_status = GPS_FIX_NONE;
                break;
        }
        xSemaphoreGive(gps_data_mutex);
    }
}

// Parses a GSV sentence for number of satellites
void parse_gsv(char* sentence) {
    char* context;
    strtok_r(sentence, ",", &context); // Sentence type ($GPGSV)
    strtok_r(NULL, ",", &context); // Total number of messages
    strtok_r(NULL, ",", &context); // Message number
    int sats = parse_nmea_int(&context);

    if (xSemaphoreTake(gps_data_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        last_gps_data.satellites = sats;
        xSemaphoreGive(gps_data_mutex);
    }
}

// Parses a GPRMC sentence
void parse_gprmc(char* sentence) {
    GpsData data;
    char* context;

    char* sentence_type = strtok_r(sentence, ",", &context);
    if (!sentence_type) return;

    char* time_token = strtok_r(NULL, ",", &context);
    
    char* validity = strtok_r(NULL, ",", &context);
    if (validity && validity[0] == 'A') {
        data.is_valid = true;
    } else {
        data.is_valid = false;
    }

    float lat = parse_nmea_float(&context);
    char* ns = strtok_r(NULL, ",", &context);
    float lon = parse_nmea_float(&context);
    char* ew = strtok_r(NULL, ",", &context);
    float speed_knots = parse_nmea_float(&context);
    strtok_r(NULL, ",", &context); // Course over ground
    char* date_token = strtok_r(NULL, ",", &context);

    // --- Start Parsing ---
    parse_nmea_time(time_token, data);
    parse_nmea_date(date_token, data);

    // Convert from NMEA format (DDMM.MMMM) to Decimal Degrees
    data.latitude = floor(lat / 100) + fmod(lat, 100) / 60.0;
    if (ns && ns[0] == 'S') data.latitude = -data.latitude;

    data.longitude = floor(lon / 100) + fmod(lon, 100) / 60.0;
    if (ew && ew[0] == 'W') data.longitude = -data.longitude;

    // Convert speed from knots to km/h
    data.speed_kmh = speed_knots * 1.852;

    // Update shared data with mutex protection
    if (xSemaphoreTake(gps_data_mutex, portMAX_DELAY) == pdTRUE) {
        // Preserve fix status and satellite count from other sentences
        data.fix_status = last_gps_data.fix_status;
        data.satellites = last_gps_data.satellites;
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
                } else if (strncmp(nmea_buffer, "$GPGSA", 6) == 0) {
                    parse_gsa(nmea_buffer);
                } else if (strncmp(nmea_buffer, "$GPGSV", 6) == 0) {
                    parse_gsv(nmea_buffer);
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

bool gps_get_time(struct tm* timeinfo) {
    if (timeinfo == NULL) {
        return false;
    }

    GpsData data;
    if (gps_get_last_data(&data) && data.is_valid) {
        timeinfo->tm_year = data.year - 1900;
        timeinfo->tm_mon = data.month - 1;
        timeinfo->tm_mday = data.day;
        timeinfo->tm_hour = data.hour;
        timeinfo->tm_min = data.minute;
        timeinfo->tm_sec = data.second;
        return true;
    }

    return false;
}

GpsFixStatus gps_get_status() {
    GpsData data;
    if (gps_get_last_data(&data)) {
        if (!data.is_valid) return GPS_FIX_NONE; // No signal from RMC
        if (data.fix_status == GPS_FIX_3D) return GPS_FIX_3D;
        if (data.fix_status == GPS_FIX_2D) return GPS_FIX_2D;
        if (data.satellites > 0) return GPS_FIX_NONE; // Has sats but no fix
    }
    return GPS_FIX_NONE; // No data at all
}
