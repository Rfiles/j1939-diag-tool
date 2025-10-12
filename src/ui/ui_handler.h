/**
 * J1939 Diagnostic Tool - UI Handler Task Interface
 * 
 * Versão: 2.1.0
 */

#ifndef UI_HANDLER_H
#define UI_HANDLER_H

#include "../core/vehicle_db_handler.h"

// Enum for the different screens/states of the UI
enum ui_state_t {
    UI_STATE_LIVE_DATA,      // Generic live data
    UI_STATE_MAIN_MENU,
    UI_STATE_SHOW_DTCS,      // Show Diagnostic Trouble Codes
    UI_STATE_ECU_LIVE_DATA   // Show live data for a specific ECU
};

// Initializes and creates the UI handler task
void ui_handler_init();

/**
 * @brief Switches the UI to the ECU-specific live data screen.
 * 
 * @param ecu A pointer to the ECU to display data for.
 */
void ui_show_ecu_data(const VehicleEcu* ecu);


#endif // UI_HANDLER_H