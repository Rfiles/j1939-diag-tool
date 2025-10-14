#ifndef CLI_COMMANDS_H
#define CLI_COMMANDS_H

#include "../cli_command.h"

// Forward declarations of the command structs
extern const CliCommand help_command;
extern const CliCommand set_command;
extern const CliCommand save_command;
extern const CliCommand get_hw_id_command;
extern const CliCommand get_license_info_command;
extern const CliCommand activate_command;
extern const CliCommand genkey_command;
extern const CliCommand dev_license_command;
extern const CliCommand request_command;
extern const CliCommand request_vin_command;

// Vehicle DB commands
extern const CliCommand list_models_command;
extern const CliCommand show_model_command;
extern const CliCommand save_db_command;
extern const CliCommand select_model_command;
extern const CliCommand read_ecu_command;

// DB Edit commands
extern const CliCommand add_model_command;
extern const CliCommand add_ecu_command;
extern const CliCommand add_pgn_command;

// DM commands
extern const CliCommand request_dm1_command;
extern const CliCommand request_dm2_command;
extern const CliCommand clear_dtcs_command;
extern const CliCommand request_dm24_command;
extern const CliCommand request_dm4_command;
extern const CliCommand request_stream_command;
extern const CliCommand request_dm11_command;
extern const CliCommand request_dm13_command;

// File transfer commands
extern const CliCommand download_command;
extern const CliCommand upload_command;
extern const CliCommand ls_command;
extern const CliCommand get_config_command;

#endif // CLI_COMMANDS_H
