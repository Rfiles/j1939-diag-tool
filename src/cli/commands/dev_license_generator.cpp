#include "dev_license_generator.h"
#include "../../core/settings_handler.h"
#include "../../core/license_handler.h"
#include <Arduino.h>
#include <cstdint>
#include <ctime>
#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>

// This file contains the logic for the hidden developer commands to generate and activate licenses.
// It is separated to keep the command definition files clean.

// --- Data structures and helpers (duplicated from license_handler.cpp for generation) ---
struct LicenseData {
    uint16_t uses;
    uint8_t  features;
    uint16_t issue_date;
    uint16_t padding;
    uint32_t signature;
};

void pack_data_for_gen(const LicenseData& data, uint8_t* buffer) {
    buffer[0] = data.uses & 0xFF;
    buffer[1] = (data.uses >> 8) & 0xFF;
    buffer[2] = data.features;
    buffer[3] = data.issue_date & 0xFF;
    buffer[4] = (data.issue_date >> 8) & 0x7F;
    uint32_t combined = (data.signature << 11) | (data.padding & 0x7FF);
    buffer[5] = combined & 0xFF;
    buffer[6] = (combined >> 8) & 0xFF;
    buffer[7] = (combined >> 16) & 0xFF;
    buffer[8] = (combined >> 24) & 0xFF;
}

uint32_t generate_truncated_signature_for_gen(const char* hw_id, uint16_t uses, uint8_t features, uint16_t issue_date) {
    mbedtls_sha256_context ctx;
    unsigned char data_to_sign[100];
    unsigned char full_signature[32];
    snprintf((char*)data_to_sign, sizeof(data_to_sign), "%s:%d:%d:%d", hw_id, uses, features, issue_date);
    mbedtls_sha256_init(&ctx);
    if (mbedtls_sha256_hmac_starts(&ctx, (const unsigned char*)"A_DIFFERENT_SECRET_KEY_FOR_V2.7_LICENSES", 39, 0) != 0) return 0;
    if (mbedtls_sha256_hmac_update(&ctx, data_to_sign, strlen((char*)data_to_sign)) != 0) return 0;
    if (mbedtls_sha256_hmac_finish(&ctx, full_signature) != 0) return 0;
    mbedtls_sha256_free(&ctx);
    return (full_signature[0] << 13) | (full_signature[1] << 5) | (full_signature[2] >> 3);
}

// --- CLI Command Implementations ---

void generate_license_key_for_cli(int uses, int features) {
    LicenseData data;
    data.uses = uses;
    data.features = features;
    time_t now;
    time(&now);
    data.issue_date = (now - 1704067200) / (24 * 3600);
    data.padding = esp_random() & 0x7FF;
    data.signature = generate_truncated_signature_for_gen(license_get_hw_id().c_str(), data.uses, data.features, data.issue_date);

    uint8_t packed_buffer[9];
    pack_data_for_gen(data, packed_buffer);

    unsigned char base64_output[13];
    size_t output_len;
    mbedtls_base64_encode(base64_output, sizeof(base64_output), &output_len, packed_buffer, sizeof(packed_buffer));
    base64_output[output_len] = '\0';

    Serial.println("--- GENERATED LICENSE KEY ---");
    Serial.printf("Key (12-char): %s\n", base64_output);
    Serial.println("-----------------------------");
}

void activate_dev_license() {
    if (settings_is_dev_cmd_disabled()) {
        Serial.println("ERROR: Developer license has already been activated for this firmware version.");
        return;
    }
    Serial.println("Generating and activating developer license...");
    LicenseData data;
    data.uses = 65535;
    data.features = 255;
    time_t now;
    time(&now);
    data.issue_date = (now - 1704067200) / (24 * 3600);
    data.padding = esp_random() & 0x7FF;
    data.signature = generate_truncated_signature_for_gen(license_get_hw_id().c_str(), data.uses, data.features, data.issue_date);

    uint8_t packed_buffer[9];
    pack_data_for_gen(data, packed_buffer);

    unsigned char base64_output[13];
    size_t output_len;
    mbedtls_base64_encode(base64_output, sizeof(base64_output), &output_len, packed_buffer, sizeof(packed_buffer));
    base64_output[output_len] = '\0';

    if (license_activate((const char*)base64_output)) {
        settings_disable_dev_cmd();
    }
}
