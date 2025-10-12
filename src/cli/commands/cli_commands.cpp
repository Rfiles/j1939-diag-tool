#include "../cli_registry.h"

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

// Function to register all commands
void cli_register_all_commands() {
    cli_register_command(&help_command);
    cli_register_command(&set_command);
    cli_register_command(&save_command);
    cli_register_command(&get_hw_id_command);
    cli_register_command(&get_license_info_command);
    cli_register_command(&activate_command);

    // Register hidden dev commands
    cli_register_command(&genkey_command);
    cli_register_command(&dev_license_command);
    cli_register_command(&request_command);
    cli_register_command(&request_vin_command);

    // Vehicle DB commands
    cli_register_command(&list_models_command);
    cli_register_command(&show_model_command);
    cli_register_command(&save_db_command);
    cli_register_command(&select_model_command);
    cli_register_command(&read_ecu_command);

    // DB Edit commands
    cli_register_command(&add_model_command);
    cli_register_command(&add_ecu_command);
    cli_register_command(&add_pgn_command);

    // DM commands
    cli_register_command(&request_dm1_command);
    cli_register_command(&request_dm2_command);
    cli_register_command(&clear_dtcs_command);
    cli_register_command(&request_dm24_command);
    cli_register_command(&request_dm4_command);
    cli_register_command(&request_stream_command);
    cli_register_command(&request_dm11_command);
    cli_register_command(&request_dm13_command);

    // File transfer commands
    cli_register_command(&download_command);
    cli_register_command(&upload_command);
    cli_register_command(&ls_command);
    cli_register_command(&get_config_command);
}
