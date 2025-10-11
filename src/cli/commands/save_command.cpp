#include "../cli_command.h"
#include "../../core/settings_handler.h"

// Execute function for the 'save' command
void save_execute(const std::vector<std::string>& args) {
    settings_save();
}

// Command definition
const CliCommand save_command = {
    "save",
    "Save all current settings to persistent storage",
    save_execute
};
