/**
 * J1939 Diagnostic Tool - CLI Command Registry Interface
 * 
 * Versão: 2.9.0
 */

#ifndef CLI_REGISTRY_H
#define CLI_REGISTRY_H

#include "cli_command.h"
#include <vector>
#include <string>

/**
 * @brief Registers a new command with the CLI system.
 * 
 * @param command A pointer to the command struct to register.
 */
void cli_register_command(const CliCommand* command);

/**
 * @brief Finds a registered command by its name.
 * 
 * @param name The name of the command to find.
 * @return A pointer to the command struct, or nullptr if not found.
 */
const CliCommand* cli_find_command(const std::string& name);

/**
 * @brief Executes the main help command, printing help text for all registered commands.
 */
void cli_print_full_help();

/**
 * @brief Initializes and registers all CLI commands.
 */
void cli_register_all_commands();

#endif // CLI_REGISTRY_H
