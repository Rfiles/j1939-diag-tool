/**
 * J1939 Diagnostic Tool - Centralized Error Handler Interface
 * 
 * Versão: 2.3.0
 */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

enum class ErrorLevel {
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

/**
 * @brief Reports an error or event. 
 *        This function formats the data into a JSON payload and sends it to the comms task
 *        for immediate MQTT publication. It also prints the error to the Serial console.
 * 
 * @param level The severity level of the event.
 * @param module The name of the module where the event occurred (e.g., "J1939", "UI").
 * @param message The specific error or event message.
 */
void error_report(ErrorLevel level, const char* module, const char* message);

#endif // ERROR_HANDLER_H
