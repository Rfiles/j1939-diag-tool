/**
 * J1939 Diagnostic Tool - CLI Handler Task Implementation
 * 
 * Versão: 2.7.0
 */

#include "cli_handler.h"
#include <Arduino.h>
#include <vector>
#include <string>
#include "../j1939/j1939_handler.h"
#include "../core/settings_handler.h"
#include "../core/license_handler.h"
#include "../core/config.h"
#include "../core/time_handler.h"
#include <mbedtls/base64.h>
#include <mbedtls/sha256.h>

// --- Helper Structs & Functions ---

struct LicenseData {
    uint16_t uses;
    uint8_t  features;
    uint16_t issue_date;
    uint16_t padding;
    uint32_t signature;
};

void pack_data(const LicenseData& data, uint8_t* buffer) {
    buffer[0] = data.uses & 0xFF;
    buffer[1] = (data.uses >> 8) & 0xFF;
    buffer[2] = data.features;
    buffer[3] = data.issue_date & 0xFF;
    buffer[4] = (data.issue_date >> 8) & 0x7F;
    buffer[5] = data.padding & 0xFF;
    buffer[6] = ((data.padding >> 8) & 0x0F) | ((data.signature & 0x0F) << 4);
    buffer[7] = (data.signature >> 4) & 0xFF;
    buffer[8] = (data.signature >> 12) & 0xFF;
}

uint32_t generate_truncated_signature(const char* hw_id, uint16_t uses, uint8_t features, uint16_t issue_date) {
    mbedtls_sha256_context ctx;
    unsigned char data_to_sign[100];
    unsigned char full_signature[32];
    snprintf((char*)data_to_sign, sizeof(data_to_sign), "%s:%d:%d:%d", hw_id, uses, features, issue_date);
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_hmac_starts_ret(&ctx, (const unsigned char*)"A_DIFFERENT_SECRET_KEY_FOR_V2.7_LICENSES", strlen("A_DIFFERENT_SECRET_KEY_FOR_V2.7_LICENSES"), 0);
    mbedtls_sha256_hmac_update_ret(&ctx, data_to_sign, strlen((char*)data_to_sign));
    mbedtls_sha256_hmac_finish_ret(&ctx, full_signature);
    mbedtls_sha256_free(&ctx);
    return (full_signature[0] << 13) | (full_signature[1] << 5) | (full_signature[2] >> 3);
}

void print_help() {
    Serial.println("--- J1939 Diag Tool CLI v2.7 ---");
    Serial.println("  set <k> <v>          - Set a config value");
    Serial.println("  save                 - Save settings to persistent storage");
    Serial.println("  get_hw_id            - Get hardware ID for licensing");
    Serial.println("  get_license_info     - Show current license status");
    Serial.println("  activate <license>   - Activate a license");
    Serial.println("  help                 - Shows this message");
}

// --- DEBUG COMMAND ---
void generate_license_key(int uses, int features) {
    LicenseData data;
    data.uses = uses;
    data.features = features;
    time_t now;
    time(&now);
    data.issue_date = (now - 1704067200) / (24 * 3600);
    data.padding = esp_random() & 0x0FFF;
    data.signature = generate_truncated_signature(license_get_hw_id().c_str(), data.uses, data.features, data.issue_date);

    uint8_t packed_buffer[9];
    pack_data(data, packed_buffer);

    unsigned char base64_output[13];
    size_t output_len;
    mbedtls_base64_encode(base64_output, sizeof(base64_output), &output_len, packed_buffer, sizeof(packed_buffer));
    base64_output[output_len] = '\0';

    Serial.println("--- GENERATED LICENSE KEY ---");
    Serial.printf("Key (12-char): %s\n", base64_output);
    Serial.println("-----------------------------");
}

void execute_command(char* command) {
    std::vector<std::string> args;
    char* token = strtok(command, " ");
    while (token != NULL) {
        args.push_back(token);
        token = strtok(NULL, " ");
    }

    if (args.empty()) return;

    if (args[0] == "help") {
        print_help();
    } else if (args[0] == "get_hw_id") {
        Serial.printf("Hardware ID: %s\n", license_get_hw_id().c_str());
    } else if (args[0] == "get_license_info") {
        Serial.printf("Remaining Uses: %d\n", license_get_remaining_uses());
        Serial.printf("Telemetry Active: %s\n", license_is_feature_active(LicensedFeature::TELEMETRY) ? "Yes" : "No");
    } else if (args[0] == "activate" && args.size() > 1) {
        license_activate(args[1].c_str());
    } else if (args[0] == "_genkey" && args.size() > 2) { // Hidden command
        generate_license_key(std::stoi(args[1]), std::stoi(args[2]));
    } else if (args[0] == "_dev_license") { // Hidden command
        if (settings_is_dev_cmd_disabled()) {
            Serial.println("ERROR: Developer license has already been activated for this firmware version.");
            return;
        }
        Serial.println("Generating and activating developer license...");
        LicenseData data;
        data.uses = 65535; // Max uses
        data.features = 255; // All features
        time_t now;
        time(&now);
        data.issue_date = (now - 1704067200) / (24 * 3600);
        data.padding = esp_random() & 0x0FFF;
        data.signature = generate_truncated_signature(license_get_hw_id().c_str(), data.uses, data.features, data.issue_date);

        uint8_t packed_buffer[9];
        pack_data(data, packed_buffer);

        unsigned char base64_output[13];
        size_t output_len;
        mbedtls_base64_encode(base64_output, sizeof(base64_output), &output_len, packed_buffer, sizeof(packed_buffer));
        base64_output[output_len] = '\0';

        if (license_activate((const char*)base64_output)) {
            settings_disable_dev_cmd(); // Disable after successful activation
        }
    } else if (args[0] == "save") {
        settings_save();
    } else if (args[0] == "set" && args.size() > 2) {
        // ... (set command logic remains the same)
    } else {
        Serial.println("Unknown command. Type 'help'.");
    }
}

// ... (cli_task_fn and cli_handler_init remain the same) ...