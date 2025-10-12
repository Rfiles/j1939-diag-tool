#ifndef J1939_PGN_DEFINITIONS_H
#define J1939_PGN_DEFINITIONS_H

#include <map>
#include <vector>
#include <cstdint>

// This file provides a basic database for the structure of common PGNs.

// Maps a PGN to the list of SPNs it contains
static std::map<uint32_t, std::vector<uint32_t>> pgn_to_spns_map = {
    {61444, {190, 92}}, // EEC1: Engine Speed, Percent Load
    {65262, {110, 175}}, // ET1: Engine Coolant Temp, Engine Oil Temp
    {65263, {100}},      // EFP: Engine Oil Pressure
    {65265, {84, 91, 521}}, // CCVS1: Vehicle Speed, Accel Pedal Pos, Brake Pedal Pos
    {65266, {183}},      // LFE: Engine Fuel Rate
    {65270, {102, 105}}, // IC1: Intake Manifold Pressure & Temp
    {65271, {168}},      // VD: Vehicle Voltage
    {65276, {96}},        // LFC1: Fuel Level
    {65235, {}},          // DM11: Memory access response
    {65237, {}},           // DM13: Stop/start broadcast
    {65228, {}}           // DM3: Clear Previously Active DTCs
};

// Reverse map for convenience: maps an SPN to its parent PGN
static std::map<uint32_t, uint32_t> spn_to_pgn_map;

// Initializes the reverse map. Should be called once on startup.
static inline void init_spn_to_pgn_map() {
    if (spn_to_pgn_map.empty()) {
        for (const auto& pair : pgn_to_spns_map) {
            for (uint32_t spn : pair.second) {
                spn_to_pgn_map[spn] = pair.first;
            }
        }
    }
}

#endif // J1939_PGN_DEFINITIONS_H
