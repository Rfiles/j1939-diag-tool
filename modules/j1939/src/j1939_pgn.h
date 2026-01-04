/**
 * @file j1939_pgn.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - PGN Definitions
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef J1939_PGN_H
#define J1939_PGN_H

/**
 * @brief PGN for requesting another PGN
 */
#define J1939_PGN_REQUEST 59904 // 0xEA00

/**
 * @brief PGN for Address Claiming
 */
#define J1939_PGN_ADDRESS_CLAIM 60928 // 0xEE00

/**
 * @brief PGN for Engine Hours, Revolutions
 */
#define J1939_PGN_ENGINE_HOURS 65253 // 0xFEE5

/**
 * @brief PGN for Vehicle Speed
 */
#define J1939_PGN_VEHICLE_SPEED 65265 // 0xFEF1

/**
 * @brief PGN for Fuel Consumption
 */
#define J1939_PGN_FUEL_CONSUMPTION 65257 // 0xFEE9

/**
 * @brief PGN for Engine Temperature
 */
#define J1939_PGN_ENGINE_TEMPERATURE 65262 // 0xFEEE

/**
 * @brief PGN for Engine Oil Pressure
 */
#define J1939_PGN_ENGINE_OIL_PRESSURE 65263 // 0xFEEF

/**
 * @brief PGN for DM1 - Active DTCs
 */
#define J1939_PGN_DM1 65226 // 0xFECA

/**
 * @brief PGN for DM2 - Previously Active DTCs
 */
#define J1939_PGN_DM2 65227 // 0xFECB


#endif // J1939_PGN_H
