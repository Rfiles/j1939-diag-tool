/**
 * J1939 Diagnostic Tool - CLI Output Stream Manager Implementation
 * 
 * Versão: 3.12.0
 */

#include "cli_output.h"
#include <stdarg.h>

// Pointer to the current stream being used for CLI output
static Stream* current_output_stream = &Serial;

void cli_set_output_stream(Stream* stream) {
    if (stream != nullptr) {
        current_output_stream = stream;
    } else {
        current_output_stream = &Serial; // Default to Serial if stream is null
    }
}

Stream* cli_get_output_stream() {
    return current_output_stream;
}

void cli_printf(const char* format, ...) {
    if (current_output_stream == nullptr) return;

    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    current_output_stream->print(buffer);
}
