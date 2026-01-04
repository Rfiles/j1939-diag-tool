/**
 * @file cli_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - CLI Handler Task Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CLI_HANDLER_H
#define CLI_HANDLER_H

#include <string>

/**
 * @brief Enum for the state of the CLI handler.
 */
enum CliState {
    CLI_STATE_NORMAL,    /**< Normal operation, waiting for commands */
    CLI_STATE_UPLOADING  /**< Waiting for file content to be uploaded */
};

/**
 * @brief Initializes and creates the CLI handler task.
 * 
 * This function initializes the CLI handler and creates a new FreeRTOS task
 * to handle the command line interface.
 */
void cli_handler_init();

/**
 * @brief Parses and executes a single command line string.
 * 
 * @param command The command line string to execute.
 */
void execute_command(char* command);

/**
 * @brief Enters the file upload mode.
 * 
 * @param filepath The path of the file to upload.
 */
void cli_enter_upload_mode(const std::string& filepath);

#endif // CLI_HANDLER_H
