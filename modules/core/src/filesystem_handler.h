/**
 * @file filesystem_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Filesystem Handler Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef FILESYSTEM_HANDLER_H
#define FILESYSTEM_HANDLER_H

/**
 * @brief Initializes LittleFS.
 * 
 * This function initializes the LittleFS filesystem.
 * 
 * @return true if initialization is successful, false otherwise.
 */
bool filesystem_init();


/**
 * @brief Formats the filesystem and creates default files.
 * 
 * This function formats the LittleFS filesystem and creates the default
 * configuration files.
 * 
 * @return true if the format and creation is successful, false otherwise.
 */
bool filesystem_format_and_create_defaults();

#endif // FILESYSTEM_HANDLER_H
