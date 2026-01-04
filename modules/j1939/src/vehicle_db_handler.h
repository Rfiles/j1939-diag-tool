/**
 * @file vehicle_db_handler.h
 * @author R. Reis
 * @date 2023-08-01
 * @brief J1939 Diagnostic Tool - Vehicle Database Handler Interface
 * 
 * @see https://github.com/ReisR/J1939-Diagnostic-Tool
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef VEHICLE_DB_HANDLER_H
#define VEHICLE_DB_HANDLER_H

#include <Arduino.h>
#include <string>
#include <vector>

/**
 * @brief Represents a single ECU in a vehicle model.
 */
struct VehicleEcu {
    String name;
    uint8_t source_address;
    std::vector<uint32_t> pgns_of_interest;
};

/**
 * @brief Represents a single vehicle model.
 */
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

/**
 * @brief Sets the currently active vehicle model by name.
 * 
 * @param model_name The name of the model to set as active.
 * @return True if the model was found and set, false otherwise.
 */
bool vehicle_db_set_active_model(const std::string& model_name);

/**
 * @brief Gets a pointer to the currently active vehicle model.
 * 
 * @return A const pointer to the active model, or nullptr if no model is active.
 */
const VehicleModel* vehicle_db_get_active_model();

/**
 * @brief Adds a new vehicle model to the database.
 * 
 * @param new_model The VehicleModel object to add.
 * @return True on success, false if a model with the same name already exists.
 */
bool vehicle_db_add_model(const VehicleModel& new_model);

/**
 * @brief Adds a new ECU to a specific vehicle model.
 * 
 * @param model_name The name of the model to modify.
 * @param new_ecu The VehicleEcu object to add.
 * @return True on success, false if the model is not found or ECU already exists.
 */
bool vehicle_db_add_ecu_to_model(const std::string& model_name, const VehicleEcu& new_ecu);

/**
 * @brief Adds a PGN of interest to a specific ECU within a model.
 * 
 * @param model_name The name of the model to modify.
 * @param ecu_name The name of the ECU to modify.
 * @param pgn The PGN to add.
 * @return True on success, false if the model or ECU is not found.
 */
bool vehicle_db_add_pgn_to_ecu(const std::string& model_name, const std::string& ecu_name, uint32_t pgn);


#endif // VEHICLE_DB_HANDLER_H