/**
 * J1939 Diagnostic Tool - DM (Diagnostic Message) Handler Interface
 * 
 * Versão: 3.6.0
 */

#ifndef DM_HANDLER_H
#define DM_HANDLER_H

#include <Arduino.h>

// Represents a single J1939 Diagnostic Trouble Code (DTC)
struct DTC {
    uint32_t spn; // Suspect Parameter Number (19 bits)
    uint8_t fmi;  // Failure Mode Identifier (5 bits)
    uint8_t oc;   // Occurrence Count (7 bits)
    uint8_t cm;   // SPN Conversion Method (1 bit)
};

/**
 * @brief Parses a raw DM1 message payload (PGN 65226).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm1(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM2 message payload (PGN 65227).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm2(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM24 message payload (PGN 65238).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm24(const uint8_t* data, uint16_t length);

// Represents a Freeze Frame of SPN data associated with a DTC
struct FreezeFrame {
    DTC trigger_dtc;
    std::map<uint32_t, float> spn_values;
};

/**
 * @brief Parses a raw DM4 message payload (PGN 65229).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm4(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM25 message payload (PGN 64951).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm25(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM11 message payload (PGN 65235).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm11(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM13 message payload (PGN 65237).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm13(const uint8_t* data, uint16_t length);


#endif // DM_HANDLER_H
