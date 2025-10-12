#include "../cli_command.h"
#include "../../j1939/j1939_handler.h"
#include "../cli_output.h"
#include <Arduino.h>

// Execute function for the 'request' command
void request_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: request <pgn>\n");
        return;
    }
    uint32_t pgn = std::stoul(args[1]);
    J1939TxRequest tx_request = { pgn, 0xFF }; // 0xFF for global request
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        cli_printf("Failed to send PGN request.\n");
    } else {
        cli_printf("PGN %lu request sent.\n", pgn);
    }
}

// Command definition
const CliCommand request_command = {
    "request",
    "Request a specific PGN from the bus",
    request_execute
};

