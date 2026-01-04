/**
 * @file dev_license_generator.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Developer License Generator
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef DEV_LICENSE_GENERATOR_H
#define DEV_LICENSE_GENERATOR_H

/**
 * @brief Generates a license key for testing purposes and prints it to the console.
 * 
 * @param uses The number of uses to encode in the key.
 * @param features The feature bitmask to encode.
 */
void generate_license_key_for_cli(int uses, int features);

/**
 * @brief Generates and activates a developer license with unlimited uses and all features.
 */
void activate_dev_license();

#endif // DEV_LICENSE_GENERATOR_H
