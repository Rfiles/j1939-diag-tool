/**
 * J1939 Diagnostic Tool - File Transfer Commands
 * 
 * Versão: 4.3.0
 */

#include "../cli_command.h"
#include "../cli_handler.h" // For cli_enter_upload_mode
#include "../cli_output.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <mbedtls/base64.h>

// --- ls ---
void ls_execute(const std::vector<std::string>& args) {
    File root = LittleFS.open("/");
    if (!root) {
        cli_printf("ERROR: Could not open root directory\n");
        return;
    }
    if (!root.isDirectory()) {
        cli_printf("ERROR: Not a directory\n");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        cli_printf("FILE: %s, SIZE: %d\n", file.name(), file.size());
        file = root.openNextFile();
    }
}

extern const CliCommand ls_command = {
    "ls",
    "List files in the root directory",
    ls_execute
};


// --- download ---
void download_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: download <filepath>\n");
        return;
    }

    File file = LittleFS.open(args[1].c_str(), "r");
    if (!file) {
        cli_printf("ERROR: File not found\n");
        return;
    }

    cli_printf("DOWNLOAD_START\n");

    unsigned char buffer[64];
    unsigned char base64_buffer[128];
    size_t bytes_read = 0;
    size_t output_len;

    Stream* out = cli_get_output_stream();
    while ((bytes_read = file.read(buffer, sizeof(buffer))) > 0) {
        mbedtls_base64_encode(base64_buffer, sizeof(base64_buffer), &output_len, buffer, bytes_read);
        out->write(base64_buffer, output_len);
        out->write('\n');
    }

    file.close();
    cli_printf("DOWNLOAD_END\n");
}

extern const CliCommand download_command = {
    "download",
    "Download a file from the device (Base64)",
    download_execute
};

// --- upload ---
void upload_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        Serial.println("Usage: upload <filepath>");
        return;
    }
    cli_enter_upload_mode(args[1]);
}

extern const CliCommand upload_command = {
    "upload",
    "Upload a file to the device (Base64)",
    upload_execute
};
