#include "../cli_command.h"
#include "../../j1939/j1939_handler.h"
#include "../cli_output.h"
#include <Arduino.h>

// PGNs for clearing DTCs
#define PGN_DM3  65228 // Clear Previously Active DTCs
#define PGN_DM11 65235 // Clear Active DTCs

// Execute function for the 'clear_dtcs' command
void clear_dtcs_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: clear_dtcs <address>\n");
        cli_printf("  <address>: The source address of the ECU to clear.\n");
        return;
    }

    uint8_t dest_address = std::stoi(args[1]);

    cli_printf("Sending commands to clear DTCs on address %d...\n", dest_address);

    // Send DM11 Request (clear active)
    J1939TxRequest tx_request_dm11 = { PGN_DM11, dest_address };
    if (xQueueSend(j1939_tx_queue, &tx_request_dm11, 0) != pdPASS) {
        cli_printf(" - Failed to send DM11 request.\n");
    } else {
        cli_printf(" - DM11 (clear active) request sent.\n");
    }

    // Send DM3 Request (clear previously active)
    J1939TxRequest tx_request_dm3 = { PGN_DM3, dest_address };
    if (xQueueSend(j1939_tx_queue, &tx_request_dm3, 0) != pdPASS) {
        cli_printf(" - Failed to send DM3 request.\n");
    } else {
        cli_printf(" - DM3 (clear previously active) request sent.\n");
    }
}

// Command definition
const CliCommand clear_dtcs_command = {
    "clear_dtcs",
    "Clear active (DM1) and previously active (DM2) DTCs from an ECU",
    clear_dtcs_execute
};

