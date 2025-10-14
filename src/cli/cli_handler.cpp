/**
 * J1939 Diagnostic Tool - CLI Handler (Serial and Telnet)
 * 
 * Versão: 3.8.0
 */

#include "cli_handler.h"
#include "cli_registry.h"
#include "cli_output.h"
#include <Arduino.h>
#include <vector>
#include <string>
#include <LittleFS.h>
#include <mbedtls/base64.h>

// --- Module State for Uploads ---
static CliState current_cli_state = CLI_STATE_NORMAL;
static File upload_file;
static unsigned long upload_timer = 0;

// --- Public API ---

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
        cli_printf("Unknown command. Type 'help'.\n");
    }
}

void cli_enter_upload_mode(const std::string& filepath) {
    upload_file = LittleFS.open(filepath.c_str(), "w");
    if (!upload_file) {
        cli_printf("ERROR: Could not open file for writing.\n");
        return;
    }
    current_cli_state = CLI_STATE_UPLOADING;
    upload_timer = millis();
    cli_printf("READY_FOR_UPLOAD\n");
    cli_printf("Send Base64 encoded lines. End with a line containing only 'UPLOAD_END'.\n");
}

// --- RTOS Task for Serial ---

void cli_serial_task_fn(void* pv) {
    char line_buffer[512];
    int line_pos = 0;

    // Set the initial stream for any startup messages
    cli_set_output_stream(&Serial);
    cli_print_full_help();

    for (;;) {
        // Always make sure the context is Serial for this task
        cli_set_output_stream(&Serial);

        if (current_cli_state == CLI_STATE_UPLOADING) {
            // Handle upload logic if initiated from Serial
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n' || c == '\r') {
                    if (line_pos > 0) {
                        line_buffer[line_pos] = '\0';
                        if (strcmp(line_buffer, "UPLOAD_END") == 0) {
                            upload_file.close();
                            cli_printf("\nSUCCESS: File upload complete.\n");
                            current_cli_state = CLI_STATE_NORMAL;
                        } else {
                            unsigned char decoded_buffer[256];
                            size_t decoded_len;
                            mbedtls_base64_decode(decoded_buffer, sizeof(decoded_buffer), &decoded_len, (const unsigned char*)line_buffer, strlen(line_buffer));
                            if (decoded_len > 0) upload_file.write(decoded_buffer, decoded_len);
                            cli_printf(".");
                        }
                        line_pos = 0;
                    }
                } else if (line_pos < sizeof(line_buffer) - 1) {
                    line_buffer[line_pos++] = c;
                }
                upload_timer = millis();
            }
            if (millis() - upload_timer > 20000) { // 20s timeout
                upload_file.close();
                current_cli_state = CLI_STATE_NORMAL;
                cli_printf("\nERROR: Upload timed out.\n");
            }
        } else { // CLI_STATE_NORMAL
            if (Serial.available()) {
                char c = Serial.read();
                if (c == '\n' || c == '\r') {
                    if (line_pos > 0) {
                        line_buffer[line_pos] = '\0';
                        cli_printf("\n");
                        execute_command(line_buffer);
                        line_pos = 0;
                    }
                    cli_printf("> ");
                } else if (c == '\b' && line_pos > 0) {
                    line_pos--;
                    Serial.print("\b \b");
                } else if (isPrintable(c) && line_pos < sizeof(line_buffer) - 1) {
                    line_buffer[line_pos++] = c;
                    Serial.print(c);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void cli_handler_init() {
    cli_register_all_commands();
    xTaskCreate(cli_serial_task_fn, "Serial CLI Task", 8192, NULL, 4, NULL);
}
