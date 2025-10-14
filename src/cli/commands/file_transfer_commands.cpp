/**
 * J1939 Diagnostic Tool - File Transfer Commands
 * 
 * Versão: 3.8.0
 */

#include "../cli_command.h"
#include "../cli_handler.h" // For cli_enter_upload_mode
#include "../cli_output.h"
#include <Arduino.h>
#include <LittleFS.h>
#include <mbedtls/base64.h>

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

    size_t file_size = file.size();
    cli_printf("FILE_BEGIN %d\n", file_size);

    unsigned char buffer[128];
    unsigned char base64_buffer[256];
    size_t bytes_read = 0;
    size_t output_len;

    Stream* out = cli_get_output_stream();
    while ((bytes_read = file.read(buffer, sizeof(buffer))) > 0) {
        mbedtls_base64_encode(base64_buffer, sizeof(base64_buffer), &output_len, buffer, bytes_read);
        out->write(base64_buffer, output_len);
    }

    file.close();
    cli_printf("\nFILE_END\n");
}

const CliCommand download_command = {
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

const CliCommand upload_command = {
    "upload",
    "Upload a file to the device (Base64)",
    upload_execute
};