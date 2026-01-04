/**
 * @file pdu_processor.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - PDU Processor Task Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PDU_PROCESSOR_H
#define PDU_PROCESSOR_H

/**
 * @brief Initializes and creates the PDU processor task.
 * 
 * This task is responsible for processing received J1939 PDUs (Protocol Data Units),
 * which are complete messages, potentially reassembled from multiple CAN frames by the
 * transport protocol handler.
 */
void pdu_processor_init();

#endif // PDU_PROCESSOR_H