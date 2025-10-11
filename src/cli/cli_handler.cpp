/**
 * J1939 Diagnostic Tool - CLI Handler Task (Refactored)
 * 
 * Versão: 2.9.0
 */

#include "cli_handler.h"
#include "cli_registry.h"
#include <Arduino.h>
#include <vector>
#include <string>

// The main execution function for the CLI
void execute_command(char* command) {
    std::vector<std::string> args;
    char* token = strtok(command, " ");
    while (token != NULL) {
        args.push_back(token);
        token = strtok(NULL, " ");
    }

    if (args.empty()) return;

    const CliCommand* cmd = cli_find_command(args[0]);
    if (cmd != nullptr) {
        cmd->execute(args);
    } else {
        Serial.println("Unknown command. Type 'help'.");
    }
}

// The FreeRTOS task function for the CLI
void cli_task_fn(void* pvParameters) {
    char cmd_buffer[256];
    int cmd_pos = 0;

    // Print the initial help message
    cli_print_full_help();

    for (;;) {
        if (Serial.available() > 0) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                if (cmd_pos > 0) {
                    cmd_buffer[cmd_pos] = '\0';
                    execute_command(cmd_buffer);
                    cmd_pos = 0;
                }
            } else if (c == '\b' && cmd_pos > 0) { // Handle backspace
                cmd_pos--;
                Serial.print("\b \b"); // Erase character from terminal
            } else if (isPrintable(c) && cmd_pos < sizeof(cmd_buffer) - 1) {
                cmd_buffer[cmd_pos++] = c;
                Serial.print(c); // Echo character
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// Public API to initialize the CLI system
void cli_handler_init() {
    cli_register_all_commands();
    xTaskCreate(cli_task_fn, "CLI Task", 4096, NULL, 4, NULL);
}
