/**
 * J1939 Diagnostic Tool - CLI Command Registry Implementation
 * 
 * Versão: 2.9.0
 */

#include "cli_registry.h"
#include "cli_output.h"

// A vector to store pointers to all registered commands
static std::vector<const CliCommand*> command_registry;

void cli_register_command(const CliCommand* command) {
    if (command != nullptr) {
        command_registry.push_back(command);
    }
}

const CliCommand* cli_find_command(const std::string& name) {
    for (const auto* cmd : command_registry) {
        if (cmd != nullptr && cmd->name == name) {
            return cmd;
        }
    }
    return nullptr;
}

void cli_print_full_help() {
    cli_printf("--- J1939 Diag Tool CLI v3.12.0 ---\n");
    cli_printf("Available commands:\n");
    for (const auto* cmd : command_registry) {
        if (cmd != nullptr) {
            // Do not show hidden commands in help
            if (cmd->name[0] != '_') {
                cli_printf("  %-20s - %s\n", cmd->name, cmd->help_text);
            }
        }
    }
}

