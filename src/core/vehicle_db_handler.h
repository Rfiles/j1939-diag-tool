/**
 * J1939 Diagnostic Tool - Vehicle Database Handler Interface
 * 
 * Versão: 3.3.0
 */

#ifndef VEHICLE_DB_HANDLER_H
#define VEHICLE_DB_HANDLER_H

#include <Arduino.h>
#include <vector>

// Represents a single ECU in a vehicle model
struct VehicleEcu {
    String name;
    uint8_t source_address;
    std::vector<uint32_t> pgns_of_interest;
};

// Represents a single vehicle model
struct VehicleModel {
    String model_name;
    String type; // "diesel" or "electric"
    std::vector<VehicleEcu> ecus;
};

/**
 * @brief Initializes the vehicle database by loading it from LittleFS.
 * 
 * @return True if the database was loaded successfully, false otherwise.
 */
bool vehicle_db_init();

/**
 * @brief Saves the current state of the vehicle database back to LittleFS.
 * 
 * @return True if the database was saved successfully, false otherwise.
 */
bool vehicle_db_save();

/**
 * @brief Gets a read-only reference to the list of all vehicle models.
 * 
 * @return A const reference to the vector of vehicle models.
 */
const std::vector<VehicleModel>& vehicle_db_get_all_models();


#endif // VEHICLE_DB_HANDLER_H
