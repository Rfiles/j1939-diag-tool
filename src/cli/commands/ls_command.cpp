#include "../cli_command.h"
#include "../cli_output.h"
#include <Arduino.h>
#include <LittleFS.h>

// Execute function for the 'ls' command
void ls_execute(const std::vector<std::string>& args) {
    File root = LittleFS.open("/");
    if (!root) {
        cli_printf("ERROR: Could not open root directory.\n");
        return;
    }
    File file = root.openNextFile();
    cli_printf("% -25s %s\n", "FILENAME", "SIZE");
    cli_printf("------------------------------------\n");
    while(file){
        if (!file.isDirectory()) {
            cli_printf("% -25s %d bytes\n", file.name(), file.size());
        }
        file = root.openNextFile();
    }
    cli_printf("------------------------------------\n");
}

// Command definition
const CliCommand ls_command = {
    "ls",
    "List files in the root directory",
    ls_execute
};

