#include "../cli_command.h"
#include "../../core/license_handler.h"
#include "../cli_output.h"
#include <Arduino.h>

// --- get_hw_id ---
void get_hw_id_execute(const std::vector<std::string>& args) {
    cli_printf("Hardware ID: %s\n", license_get_hw_id().c_str());
}
extern const CliCommand get_hw_id_command = {
    "get_hw_id",
    "Get the unique hardware ID for licensing",
    get_hw_id_execute
};

// --- get_license_info ---
void get_license_info_execute(const std::vector<std::string>& args) {
    cli_printf("Remaining Uses: %d\n", license_get_remaining_uses());
    cli_printf("Telemetry Active: %s\n", license_is_feature_active(LicensedFeature::TELEMETRY) ? "Yes" : "No");
}
extern const CliCommand get_license_info_command = {
    "get_license_info",
    "Show current license status",
    get_license_info_execute
};

// --- activate ---
void activate_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: activate <license_key>\n");
        return;
    }
    license_activate(args[1].c_str());
}
extern const CliCommand activate_command = {
    "activate",
    "Activate a license (e.g., activate <key>)",
    activate_execute
};

// --- Hidden dev commands ---
#include <mbedtls/base64.h>
#include "../commands/dev_license_generator.h" // Include the generator logic

void genkey_execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        cli_printf("Usage: _genkey <uses> <features>\n");
        return;
    }
    generate_license_key_for_cli(std::stoi(args[1]), std::stoi(args[2]));
}
extern const CliCommand genkey_command = {
    "_genkey",
    "(Dev) Generate a license key",
    genkey_execute
};

void dev_license_execute(const std::vector<std::string>& args) {
    activate_dev_license();
}
extern const CliCommand dev_license_command = {
    "_dev_license",
    "(Dev) Activate a developer license",
    dev_license_execute
};
