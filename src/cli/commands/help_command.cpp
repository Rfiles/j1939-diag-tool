#include "../cli_command.h"
#include "../cli_registry.h"

// Execute function for the 'help' command
void help_execute(const std::vector<std::string>& args) {
    cli_print_full_help();
}

// Command definition
const CliCommand help_command = {
    "help",
    "Shows this help message",
    help_execute
};
