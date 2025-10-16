#include "../cli_command.h"
#include "../../j1939/tp_handler.h"
#include "../cli_output.h"
#include <Arduino.h>

// PGN for DM21 (Data Stream Request)
#define PGN_DM21 65234

// Execute function for the 'request_stream' command
void request_stream_execute(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        cli_printf("Usage: request_stream <address> <spn1> [spn2] ...\n");
        return;
    }

    uint8_t dest_address = std::stoi(args[1]);
    
    // The data payload is a list of SPNs (3 bytes each, little-endian)
    int num_spns = args.size() - 2;
    std::vector<uint8_t> data_payload;
    data_payload.reserve(num_spns * 3);

    for (int i = 0; i < num_spns; ++i) {
        uint32_t spn = std::stoul(args[i + 2]);
        data_payload.push_back(spn & 0xFF);
        data_payload.push_back((spn >> 8) & 0xFF);
        data_payload.push_back((spn >> 16) & 0xFF);
    }

    tp_send_message(PGN_DM21, dest_address, data_payload.data(), data_payload.size());
    cli_printf("DM21 Data Stream Request sent to address %d.\n", dest_address);
}

// Command definition
extern const CliCommand request_stream_command = {
    "request_stream",
    "Request a data stream of specific SPNs from an ECU (DM21)",
    request_stream_execute
};

