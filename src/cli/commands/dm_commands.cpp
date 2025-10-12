#include "../cli_command.h"
#include "../../j1939/j1939_handler.h"
#include "../cli_output.h"

// PGNs for Diagnostic Messages
#define PGN_DM1 65226
#define PGN_DM2 65227

// --- request_dm1 ---
void request_dm1_execute(const std::vector<std::string>& args) {
    J1939TxRequest tx_request = { PGN_DM1, 0xFF }; // Global request
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        cli_printf("Failed to send DM1 request.\n");
    } else {
        cli_printf("DM1 request sent.\n");
    }
}
const CliCommand request_dm1_command = {
    "request_dm1",
    "Request active diagnostic trouble codes (DM1)",
    request_dm1_execute
};

// --- request_dm2 ---
void request_dm2_execute(const std::vector<std::string>& args) {
    J1939TxRequest tx_request = { PGN_DM2, 0xFF }; // Global request
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        cli_printf("Failed to send DM2 request.\n");
    } else {
        cli_printf("DM2 request sent.\n");
    }
}
const CliCommand request_dm2_command = {
    "request_dm2",
    "Request previously active DTCs (DM2)",
    request_dm2_execute
};

// --- request_dm24 ---
#define PGN_DM24 65238
void request_dm24_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: request_dm24 <address>\n");
        return;
    }
    uint8_t dest_address = std::stoi(args[1]);
    J1939TxRequest tx_request = { PGN_DM24, dest_address };
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        cli_printf("Failed to send DM24 request.\n");
    } else {
        cli_printf("DM24 request sent to address %d.\n", dest_address);
    }
}
const CliCommand request_dm24_command = {
    "request_dm24",
    "Request supported DTCs from an ECU (DM24)",
    request_dm24_execute
};

// --- request_dm4 ---
#define PGN_DM4 65229
void request_dm4_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: request_dm4 <address>\n");
        return;
    }
    uint8_t dest_address = std::stoi(args[1]);
    J1939TxRequest tx_request = { PGN_DM4, dest_address };
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        cli_printf("Failed to send DM4 request.\n");
    } else {
        cli_printf("DM4 request sent to address %d.\n", dest_address);
    }
}
const CliCommand request_dm4_command = {
    "request_dm4",
    "Request freeze frame data from an ECU (DM4)",
    request_dm4_execute
};

// --- request_dm25 ---
#define PGN_DM25 64951
void request_dm25_execute(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        cli_printf("Usage: request_dm25 <address>\n");
        return;
    }
    uint8_t dest_address = std::stoi(args[1]);
    J1939TxRequest tx_request = { PGN_DM25, dest_address };
    if (xQueueSend(j1939_tx_queue, &tx_request, 0) != pdPASS) {
        cli_printf("Failed to send DM25 request.\n");
    } else {
        cli_printf("DM25 request sent to address %d.\n", dest_address);
    }
}
const CliCommand request_dm25_command = {
    "request_dm25",
    "Request expanded freeze frame from an ECU (DM25)",
    request_dm25_execute
};
