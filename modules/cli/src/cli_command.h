/**
 * J1939 Diagnostic Tool - CLI Command Interface
 * 
 * Versão: 2.9.0
 */

#ifndef CLI_COMMAND_H
#define CLI_COMMAND_H

#include <vector>
#include <string>

// Struct defining the interface for any CLI command
struct CliCommand {
    const char* name;        // Command name (e.g., "set")
    const char* help_text;   // A brief description of the command
    void (*execute)(const std::vector<std::string>& args); // Function pointer to the implementation
};

#endif // CLI_COMMAND_H
