/**
 * @file hardware_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Hardware Handler
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef HARDWARE_HANDLER_H
#define HARDWARE_HANDLER_H

/**
 * @brief Initializes the hardware handler.
 * 
 * This function initializes any hardware-specific components.
 */
void hardware_handler_init();

/**
 * @brief The main loop for the hardware handler.
 * 
 * This function is called repeatedly to handle any hardware-specific tasks.
 */
void hardware_handler_loop();

#endif // HARDWARE_HANDLER_H