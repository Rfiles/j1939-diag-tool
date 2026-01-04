/**
 * J1939 Diagnostic Tool - CLI Handler Task Interface
 * 
 * Versão: 3.8.0
 */

#ifndef CLI_HANDLER_H
#define CLI_HANDLER_H

#include <string>

// Enum for the state of the CLI handler
enum CliState {
    CLI_STATE_NORMAL,
    CLI_STATE_UPLOADING
};

// Initializes and creates the CLI handler task
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
