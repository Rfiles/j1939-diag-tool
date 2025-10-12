/**
 * J1939 Diagnostic Tool - SPN Parser Implementation
 * 
 * Versão: 3.16.0
 */

#include "spn_parser.h"

// This function contains the decoding logic for specific SPNs.
// This would be expanded to support more SPNs as needed.
ParsedSPN spn_parse(const can_frame& frame, uint32_t spn) {
    ParsedSPN result;
    result.is_valid = true; // Assume valid unless stated otherwise

    switch (spn) {
        // --- Engine --- 
        case 190: { // Engine Speed (from EEC1, PGN 61444)
            uint16_t raw = (frame.data[4] << 8) | frame.data[3];
            result.value = raw * 0.125; // 0.125 rpm/bit
            result.units = "RPM";
            break;
        }
        case 92: { // Engine Percent Load (from EEC1, PGN 61444)
            result.value = frame.data[2]; // 1 %/bit, 0 offset
            result.units = "%";
            break;
        }
        case 110: { // Engine Coolant Temperature (from ET1, PGN 65262)
            result.value = frame.data[0] - 40.0; // 1 C/bit, -40C offset
            result.units = "C";
            break;
        }
        case 175: { // Engine Oil Temperature 1 (from ET1, PGN 65262)
            uint16_t raw = (frame.data[3] << 8) | frame.data[2];
            result.value = raw * 0.03125 - 273; // 0.03125 deg C/bit, -273C offset
            result.units = "C";
            break;
        }
        case 100: { // Engine Oil Pressure (from EFP, PGN 65263)
            result.value = frame.data[3] * 4.0; // 4 kPa/bit
            result.units = "kPa";
            break;
        }
        case 183: { // Engine Fuel Rate (from LFE, PGN 65266)
            uint16_t raw = (frame.data[1] << 8) | frame.data[0];
            result.value = raw * 0.05; // 0.05 L/h per bit
            result.units = "L/h";
            break;
        }
        case 102: { // Engine Intake Manifold #1 Pressure (from IC1, PGN 65270)
            result.value = frame.data[1] * 2.0; // 2 kPa/bit
            result.units = "kPa";
            break;
        }
        case 105: { // Engine Intake Manifold #1 Temperature (from IC1, PGN 65270)
            result.value = frame.data[2] - 40.0; // 1 C/bit, -40C offset
            result.units = "C";
            break;
        }

        // --- Vehicle --- 
        case 84: { // Wheel-Based Vehicle Speed (from CCVS1, PGN 65265)
            uint16_t raw = (frame.data[2] << 8) | frame.data[1];
            result.value = raw * (1.0/256.0); // 1/256 km/h per bit
            result.units = "km/h";
            break;
        }
        case 91: { // Accelerator Pedal Position 1 (from CCVS1, PGN 65265)
            result.value = frame.data[2] * 0.4; // 0.4 %/bit
            result.units = "%";
            break;
        }
        case 521: { // Brake Pedal Position (from CCVS1, PGN 65265)
            result.value = frame.data[3] * 0.4; // 0.4 %/bit
            result.units = "%";
            break;
        }
        case 168: { // Electrical Potential (Voltage) (from VD, PGN 65271)
            uint16_t raw = (frame.data[5] << 8) | frame.data[4];
            result.value = raw * 0.05; // 0.05 V/bit
            result.units = "V";
            break;
        }
        case 96: { // Fuel Level 1 (from LFC1, PGN 65276)
            result.value = frame.data[1] * 0.4; // 0.4 %/bit
            result.units = "%";
            break;
        }

        default: {
            result.is_valid = false;
            break;
        }
    }

    return result;
