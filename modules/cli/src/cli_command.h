/**
 * @file cli_command.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - CLI Command Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CLI_COMMAND_H
#define CLI_COMMAND_H

#include <vector>
#include <string>

/**
 * @brief Struct defining the interface for any CLI command.
 * 
 * This structure defines the name, help text, and execute function for a CLI command.
 * It allows for a modular and extensible command system.
 */
struct CliCommand {
    const char* name;        /**< Command name (e.g., "set") */
    const char* help_text;   /**< A brief description of the command */
    void (*execute)(const std::vector<std::string>& args); /**< Function pointer to the implementation */
};

#endif // CLI_COMMAND_H