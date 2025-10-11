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
extern const CliCommand request_vin_command;

// Vehicle DB commands
extern const CliCommand list_models_command;
extern const CliCommand show_model_command;
extern const CliCommand save_db_command;

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
    cli_register_command(&request_vin_command);

    // Vehicle DB commands
    cli_register_command(&list_models_command);
    cli_register_command(&show_model_command);
    cli_register_command(&save_db_command);
}
