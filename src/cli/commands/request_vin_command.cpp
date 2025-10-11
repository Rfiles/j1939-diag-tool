#include "../cli_command.h"
#include "../../j1939/j1939_handler.h"

// PGN for Vehicle Identification (VIN)
#define PGN_VEHICLE_IDENTIFICATION 65260

// Execute function for the 'request_vin' command
void request_vin_execute(const std::vector<std::string>& args) {
    J1939TxRequest tx_request = { PGN_VEHICLE_IDENTIFICATION, 0xFF }; // 0xFF for global request
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        Serial.println("Failed to send VIN request.");
    } else {
        Serial.println("VIN request sent.");
    }
}

// Command definition
const CliCommand request_vin_command = {
    "request_vin",
    "Request the Vehicle Identification Number (VIN)",
    request_vin_execute
};
