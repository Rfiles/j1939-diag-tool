/**
 * @file error_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Centralized Error Handler Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/**
 * @brief Enum for the severity level of an error or event.
 */
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