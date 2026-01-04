/**
 * J1939 Diagnostic Tool - CLI Output Stream Manager Interface
 * 
 * Versão: 3.12.0
 */

#ifndef CLI_OUTPUT_H
#define CLI_OUTPUT_H

#include <Arduino.h>

/**
 * @brief Sets the current output stream for all CLI commands.
 * 
 * @param stream A pointer to the stream to use for output (e.g., &Serial, &wifiClient).
 */
void cli_set_output_stream(Stream* stream);

/**
 * @brief Gets the current output stream.
 * 
 * @return A pointer to the current output stream.
 */
Stream* cli_get_output_stream();

/**
 * @brief A printf-style function that directs its output to the current CLI stream.
 * 
 * @param format The format string.
 * @param ... The variable arguments.
 */
void cli_printf(const char* format, ...);

#endif // CLI_OUTPUT_H
