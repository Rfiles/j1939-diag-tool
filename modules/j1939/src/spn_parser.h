/**
 * @file spn_parser.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - SPN Parser Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SPN_PARSER_H
#define SPN_PARSER_H

#include "../../can/src/mcp2515_driver.h"
#include <string>

/**
 * @brief Struct to hold a parsed SPN value and its units.
 */
struct ParsedSPN {
    bool is_valid = false;
    float value;
    std::string units;
};

/**
 * @brief Parses a specific SPN from a given CAN frame.
 * 
 * @param frame The CAN frame containing the PGN that holds the SPN.
 * @param spn The SPN to parse.
 * @return A ParsedSPN struct with the value and units.
 */
ParsedSPN spn_parse(const can_frame& frame, uint32_t spn);

#endif // SPN_PARSER_H