/**
 * J1939 Diagnostic Tool - Advanced Licensing Handler Interface
 * 
 * Versão: 2.7.0
 */

#ifndef LICENSE_HANDLER_H
#define LICENSE_HANDLER_H

#include <Arduino.h>

// Feature flags that can be licensed
enum class LicensedFeature : uint8_t {
    TELEMETRY = 1 << 0, // Bit 0
    J1939_PRO = 1 << 1, // Bit 1 (Example for a future feature)
    // Add up to 8 features here
};

/**
 * @brief Initializes the licensing system.
 */
void license_handler_init();

/**
 * @brief Gets the unique hardware ID of the device.
 * 
 * @return A string containing the hardware ID.
 */
String license_get_hw_id();

/**
 * @brief Activates a new license key.
 * 
 * @param key The 12-character license key to activate.
 * @return True if the license key is valid and was activated successfully, false otherwise.
 */
bool license_activate(const char* key);

/**
 * @brief Gets the number of remaining uses for the current license.
 * 
 * @return The number of uses left. Returns 0 if no valid license is active.
 */
int license_get_remaining_uses();

/**
 * @brief Checks if a specific feature is enabled by the current license.
 * 
 * @param feature The feature to check.
 * @return True if the feature is licensed and there are uses remaining, false otherwise.
 */
bool license_is_feature_active(LicensedFeature feature);

/**
 * @brief Consumes one use from the current license. 
 *        This should be called when a licensed feature is used.
 */
void license_consume_use();

#endif // LICENSE_HANDLER_H