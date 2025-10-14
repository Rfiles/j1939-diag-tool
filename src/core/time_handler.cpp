/**
 * J1939 Diagnostic Tool - Time Handler Implementation
 * 
 * Versão: 3.8.0
 */

#include <sys/time.h>
#include "time_handler.h"
#include "error_handler.h"
#include "../comms/gps/gps_handler.h"

// NTP server settings
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 0;
const int DAYLIGHT_OFFSET_SEC = 0;

static bool time_is_synced = false;
static TimeSource time_source = TIME_SOURCE_NTP; // Default to NTP

void time_handler_init(TimeSource source) {
    time_source = source;
    error_report(ErrorLevel::INFO, "Time", "Time handler initialized.");
}

void time_set_source(TimeSource source) {
    time_source = source;
    char msg[50];
    sprintf(msg, "Time source set to %s", source == TIME_SOURCE_NTP ? "NTP" : "GPS");
    error_report(ErrorLevel::INFO, "Time", msg);
}

bool time_sync() {
    struct tm timeinfo;
    bool success = false;

    if (time_source == TIME_SOURCE_NTP) {
        error_report(ErrorLevel::INFO, "Time", "Synchronizing time from NTP server...");
        configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
        if (getLocalTime(&timeinfo, 10000)) { // 10-second timeout
            success = true;
        }
    } else if (time_source == TIME_SOURCE_GPS) {
        error_report(ErrorLevel::INFO, "Time", "Synchronizing time from GPS...");
        if (gps_get_time(&timeinfo)) {
            time_t t = mktime(&timeinfo);
            struct timeval tv = { .tv_sec = t };
            settimeofday(&tv, NULL);
            success = true;
        }
    }

    if (success) {
        time_is_synced = true;
        error_report(ErrorLevel::INFO, "Time", "Time synchronized successfully.");
    } else {
        error_report(ErrorLevel::WARN, "Time", "Failed to synchronize time.");
    }

    return success;
}

String time_handler_get_timestamp() {
    if (!time_is_synced) {
        return "[TIME NOT SYNCED]";
    }

    char buffer[30];
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
        return String(buffer);
    } else {
        return "[TIME ERROR]";
    }
}
