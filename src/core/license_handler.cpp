/**
 * J1939 Diagnostic Tool - Advanced Licensing Handler Implementation
 * 
 * Versão: 2.7.0
 */

#include "license_handler.h"
#include "error_handler.h"
#include "time_handler.h"
#include <Preferences.h>
#include <WiFi.h>
#include <mbedtls/sha256.h>
#include <mbedtls/base64.h>
#include <mbedtls/md.h>

// --- Security Warning ---
static const char* HMAC_SECRET_KEY = "A_DIFFERENT_SECRET_KEY_FOR_V2.7_LICENSES";

// --- NVS Keys ---
static const char* NVS_LICENSE_NAMESPACE = "licensing_v2";
static const char* NVS_USES_KEY = "uses_left";
static const char* NVS_FEATURES_KEY = "features";
static const char* NVS_BRUTE_FORCE_COUNT = "bf_count";
static const char* NVS_BRUTE_FORCE_TIME = "bf_time";

// --- Brute-force Protection ---
const int MAX_FAILED_ATTEMPTS = 5;
const unsigned long COOLDOWN_PERIOD_MS = 15 * 60 * 1000; // 15 minutes

// --- Time-based Activation ---
const int ACTIVATION_WINDOW_DAYS = 7;

// --- License Data Structure (72 bits / 9 bytes) ---
struct LicenseData {
    uint16_t uses;          // 16 bits
    uint8_t  features;      // 8 bits
    uint16_t issue_date;    // 15 bits (1 bit unused)
    uint16_t padding;       // 12 bits (4 bits unused)
    uint32_t signature;     // 21 bits (11 bits unused)
};

// Epoch for date calculation (days since Jan 1, 2024)
const time_t EPOCH_START = 1704067200; // 2024-01-01 00:00:00 UTC

Preferences license_prefs;

// --- Internal State ---
static int remaining_uses = 0;
static uint8_t active_features = 0;
static bool is_initialized = false;

// --- Helper Functions ---

// Packs the license data into a 9-byte array for signing/encoding
void pack_data(const LicenseData& data, uint8_t* buffer) {
    // Bytes 0, 1: uses (16 bits)
    buffer[0] = data.uses & 0xFF;
    buffer[1] = (data.uses >> 8) & 0xFF;
    // Byte 2: features (8 bits)
    buffer[2] = data.features;
    // Byte 3, 4: issue_date (15 bits)
    buffer[3] = data.issue_date & 0xFF;
    buffer[4] = (data.issue_date >> 8) & 0x7F; // Mask to 7 bits for the high byte

    // Bytes 5-8: signature (21 bits) and padding (11 bits)
    uint32_t combined = (data.signature << 11) | (data.padding & 0x7FF);
    buffer[5] = combined & 0xFF;
    buffer[6] = (combined >> 8) & 0xFF;
    buffer[7] = (combined >> 16) & 0xFF;
    buffer[8] = (combined >> 24) & 0xFF;
}

// Unpacks a 9-byte array into the LicenseData struct
void unpack_data(const uint8_t* buffer, LicenseData& data) {
    data.uses = buffer[0] | (buffer[1] << 8);
    data.features = buffer[2];
    data.issue_date = buffer[3] | ((buffer[4] & 0x7F) << 8);

    uint32_t combined = buffer[5] | (buffer[6] << 8) | (buffer[7] << 16) | (buffer[8] << 24);
    data.signature = combined >> 11;
    data.padding = combined & 0x7FF;
}

// Generates the full 256-bit HMAC-SHA256 signature
uint32_t generate_truncated_signature(const char* hw_id, uint16_t uses, uint8_t features, uint16_t issue_date) {
    mbedtls_sha256_context ctx;
    unsigned char data_to_sign[100];
    unsigned char full_signature[32];

    snprintf((char*)data_to_sign, sizeof(data_to_sign), "%s:%d:%d:%d", hw_id, uses, features, issue_date);

    mbedtls_sha256_init(&ctx);
    if (mbedtls_sha256_hmac_starts(&ctx, (const unsigned char*)HMAC_SECRET_KEY, strlen(HMAC_SECRET_KEY), 0) != 0) return 0;
    if (mbedtls_sha256_hmac_update(&ctx, data_to_sign, strlen((char*)data_to_sign)) != 0) return 0;
    if (mbedtls_sha256_hmac_finish(&ctx, full_signature) != 0) return 0;
    mbedtls_sha256_free(&ctx);

    // Truncate the signature to 21 bits (return as uint32_t)
    return (full_signature[0] << 13) | (full_signature[1] << 5) | (full_signature[2] >> 3);
}

// --- Public API Implementation ---

void license_handler_init() {
    if (!license_prefs.begin(NVS_LICENSE_NAMESPACE, false)) {
        error_report(ErrorLevel::ERROR, "License", "Failed to initialize NVS");
        return;
    }
    remaining_uses = license_prefs.getInt(NVS_USES_KEY, 0);
    active_features = license_prefs.getUChar(NVS_FEATURES_KEY, 0);
    is_initialized = true;
}

String license_get_hw_id() {
    return WiFi.macAddress();
}

bool license_activate(const char* key) {
    // 1. Brute-force check
    unsigned long last_attempt_time = license_prefs.getULong(NVS_BRUTE_FORCE_TIME, 0);
    if (millis() - last_attempt_time < COOLDOWN_PERIOD_MS) {
        error_report(ErrorLevel::ERROR, "License", "Activation locked due to too many failed attempts.");
        return false;
    }

    // 2. Time sync check
    if (time_handler_get_timestamp().startsWith("[")) { // Check if time is not synced
        error_report(ErrorLevel::ERROR, "License", "Time not synced. Cannot activate license.");
        return false;
    }

    // 3. Decode key
    uint8_t decoded_buffer[9];
    size_t decoded_len;
    if (mbedtls_base64_decode(decoded_buffer, sizeof(decoded_buffer), &decoded_len, (const unsigned char*)key, strlen(key)) != 0 || decoded_len != 9) {
        license_prefs.putUInt(NVS_BRUTE_FORCE_COUNT, license_prefs.getUInt(NVS_BRUTE_FORCE_COUNT, 0) + 1);
        error_report(ErrorLevel::ERROR, "License", "Invalid key format.");
        return false;
    }

    LicenseData data;
    unpack_data(decoded_buffer, data);

    // 4. Activation window check
    time_t now;
    time(&now);
    uint16_t days_since_epoch = (now - EPOCH_START) / (24 * 3600);
    if (days_since_epoch > data.issue_date + ACTIVATION_WINDOW_DAYS) {
        error_report(ErrorLevel::ERROR, "License", "License key has expired.");
        return false;
    }

    // 5. Signature verification
    uint32_t expected_signature = generate_truncated_signature(license_get_hw_id().c_str(), data.uses, data.features, data.issue_date);
    if (data.signature != expected_signature) {
        uint32_t failed_count = license_prefs.getUInt(NVS_BRUTE_FORCE_COUNT, 0) + 1;
        license_prefs.putUInt(NVS_BRUTE_FORCE_COUNT, failed_count);
        if (failed_count >= MAX_FAILED_ATTEMPTS) {
            license_prefs.putULong(NVS_BRUTE_FORCE_TIME, millis());
            license_prefs.putUInt(NVS_BRUTE_FORCE_COUNT, 0);
        }
        error_report(ErrorLevel::ERROR, "License", "Invalid signature.");
        return false;
    }

    // 6. Success!
    license_prefs.putUInt(NVS_BRUTE_FORCE_COUNT, 0); // Reset counter
    license_prefs.putInt(NVS_USES_KEY, data.uses);
    license_prefs.putUChar(NVS_FEATURES_KEY, data.features);
    remaining_uses = data.uses;
    active_features = data.features;
    error_report(ErrorLevel::INFO, "License", "License activated successfully!");
    return true;
}

int license_get_remaining_uses() {
    return is_initialized ? remaining_uses : 0;
}

bool license_is_feature_active(LicensedFeature feature) {
    if (!is_initialized || remaining_uses <= 0) {
        return false;
    }
    return (active_features & (uint8_t)feature) != 0;
}

void license_consume_use() {
    if (is_initialized && remaining_uses > 0) {
        remaining_uses--;
        license_prefs.putInt(NVS_USES_KEY, remaining_uses);
    }
}