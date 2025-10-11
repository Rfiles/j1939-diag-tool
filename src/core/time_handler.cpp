/**
 * J1939 Diagnostic Tool - NTP Time Handler Implementation
 * 
 * Versão: 2.5.0
 */

#include "time_handler.h"
#include "time.h"
#include "error_handler.h"

// NTP server settings
const char* NTP_SERVER = "pool.ntp.org";
const long GMT_OFFSET_SEC = 0;      // Set your GMT offset in seconds
const int DAYLIGHT_OFFSET_SEC = 0; // Set your daylight saving offset in seconds

static bool time_is_synced = false;

void time_handler_init() {
    error_report(ErrorLevel::INFO, "Time", "Initializing NTP time synchronization...");
    // Configure time from NTP server
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

    // Give it a moment to sync
    delay(1000);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        time_is_synced = true;
        error_report(ErrorLevel::INFO, "Time", "NTP time synchronized successfully.");
    } else {
        error_report(ErrorLevel::WARN, "Time", "Failed to synchronize NTP time on first attempt.");
    }
}

String time_handler_get_timestamp() {
    if (!time_is_synced) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 100)) { // 100ms timeout
            time_is_synced = true; // Mark as synced on first success
        } else {
            return "[TIME NOT SYNCED]";
        }
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
