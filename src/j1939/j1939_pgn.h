/**
 * J1939 Diagnostic Tool - PGN Definitions
 * 
 * Versão: 2.1.0
 */

#ifndef J1939_PGN_H
#define J1939_PGN_H

// J1939 Parameter Group Numbers (PGNs)

// PGN for requesting another PGN
#define J1939_PGN_REQUEST 59904 // 0xEA00

// PGN for Address Claiming
#define J1939_PGN_ADDRESS_CLAIM 60928 // 0xEE00

// --- Common PGNs for Diagnostics & Data ---

// Engine Hours, Revolutions
#define J1939_PGN_ENGINE_HOURS 65253 // 0xFEE5

// Vehicle Speed
#define J1939_PGN_VEHICLE_SPEED 65265 // 0xFEF1

// Fuel Consumption
#define J1939_PGN_FUEL_CONSUMPTION 65257 // 0xFEE9

// Engine Temperature
#define J1939_PGN_ENGINE_TEMPERATURE 65262 // 0xFEEE

// Engine Oil Pressure
#define J1939_PGN_ENGINE_OIL_PRESSURE 65263 // 0xFEEF

// --- Diagnostic Trouble Codes (DTCs) ---
// These use the Transport Protocol, which is more complex.
// We define them here for future use.
#define J1939_PGN_DM1 65226 // 0xFECA - Active DTCs
#define J1939_PGN_DM2 65227 // 0xFECB - Previously Active DTCs


#endif // J1939_PGN_H