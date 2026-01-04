# Contexto do Projeto Gemini: Ferramenta de Diagnóstico J1939

## 1. Intenção do Projeto

O objetivo é desenvolver uma ferramenta de diagnóstico e telemática J1939 de nível profissional para a plataforma ESP32 (especificamente a placa LilyGo T-Display S3). A ferramenta deve ser robusta, modular e extensível, servindo como base para uma aplicação comercial.

**Funcionalidades Chave:**
- Comunicação completa com redes J1939.
- Interpretação de protocolos de diagnóstico complexos (DMs).
- Envio de telemetria para plataformas de IoT.
- Sistema de licenciamento de software ligado ao hardware.
- Base de dados de veículos para diagnósticos específicos por modelo.

## 2. Estilo de Programação e Arquitetura

Para garantir a robustez e modularidade, o projeto segue padrões de desenvolvimento de sistemas embarcados de alta qualidade.

- **Arquitetura Principal:** Baseada em **FreeRTOS**. Cada responsabilidade principal (comunicação CAN, CLI, UI, conectividade Wi-Fi) é isolada na sua própria tarefa, comunicando através de filas (`Queues`) para garantir um funcionamento não-bloqueante.
- **Modularidade:** O código está organizado em módulos claros e com responsabilidades bem definidas (ex: `j1939_handler`, `dm_handler`, `cli_handler`). A adição de novas funcionalidades, como comandos CLI, é feita de forma modular, sem alterar o código central.
- **Gestão de Concorrência:** Recursos partilhados (como o objeto de configuração global) são protegidos com **mutexes** para evitar condições de corrida (`race conditions`).
- **Gestão de Configuração:**
  - **Configurações Padrão:** Carregadas a partir de ficheiros `.json` no diretório `/data` (ex: `config.json`, `vehicles.json`).
  - **Configurações do Utilizador:** Dados sensíveis ou específicos (como senhas de Wi-Fi) são definidos via CLI e guardados na **NVS (Non-Volatile Storage)**, para não serem expostos no código-fonte.
- **Abstração de Hardware:** Os drivers para componentes específicos (ex: transceiver CAN MCP2515, ecrã ST7789) estão isolados na sua própria camada (`src/driver`), facilitando a adaptação a outro hardware.

Arduino esp32 core framework 3.3.2
Utilizar vector e map para programação de funções quando aplicavel.
Ser menos verboso nas respostas. Explica so se solicitado.
actualiza a versão do header caso o firmware seja atualizado. A versão será a actual do projecto.
Actualiza o changelog apos cada atualização continuando desde a versão anterior.


## 3. Funcionalidades Implementadas

Abaixo está um resumo das principais funcionalidades desenvolvidas até ao momento:

- **Stack J1939 Completa:**
  - **Address Claiming:** Negociação automática de endereço na rede J1939 para evitar conflitos.
  - **Transport Protocol (TP):** Suporte completo para receber e enviar mensagens multi-pacote (BAM e CTS), essencial para dados de diagnóstico.
  - **PDU Processor:** Processamento e encaminhamento de mensagens J1939 recebidas.

- **Diagnóstico J1939 (DM - Diagnostic Messages):**
  - **DM1/DM2:** Leitura e interpretação de códigos de erro ativos e passivos.
  - **DM3/DM11:** Comandos para limpar os códigos de erro.
  - **DM4/DM25:** Leitura de "Freeze Frame" (dados do momento da falha).
  - **DM13:** Comando para controlar a transmissão de broadcasts.
  - **DM24:** Leitura de DTCs suportados por uma ECU.
  - **Leitura de VIN:** Comando dedicado para obter o Número de Identificação do Veículo.

- **Conectividade e Telemática:**
  - **Wi-Fi e MQTT:** Conexão a redes Wi-Fi e publicação de dados de diagnóstico e telemetria para um broker MQTT.
  - **Sincronização de Tempo (NTP):** Obtenção da hora correta para timestamps precisos.
  - **Acesso Remoto:**
    - **Telnet:** Acesso completo à CLI através da rede.
    - **Servidor HTTP:** Gestão remota de ficheiros (listar, upload, download).

- **Sistema e Interface:**
  - **CLI Avançada:** Interface de linha de comandos completa, modular e acessível via Serial e Telnet.
  - **Bases de Dados:**
    - **Veículos:** Gestão de uma base de dados de veículos (`vehicles.json`) para diagnósticos específicos.
    - **SPN/FMI:** Descrições legíveis para códigos de erro, tornando o diagnóstico mais intuitivo.
  - **Sistema de Licenciamento:** Mecanismo de ativação de software com chaves assinadas e ligadas ao ID de hardware do dispositivo.
  - **Interface Gráfica (GUI):** Uma ferramenta de desktop (`gui_tool.py`) para facilitar a interação com o dispositivo, incluindo um gestor de ficheiros.
  - **Atualizações OTA:** Capacidade de atualizar o firmware remotamente.
  
## 4. Documentação dos Módulos

### 4.1. CAN

**`mcp2515_driver.h`**
```cpp
/**
 * @brief A generic CAN frame structure.
 * 
 * This structure holds the information for a single CAN frame, including the
 * CAN ID, Data Length Code (DLC), and the data payload.
 */
struct can_frame {
    uint32_t can_id;  /**< 29-bit CAN ID */
    uint8_t  can_dlc; /**< Data Length Code (0-8) */
    uint8_t  data[8]; /**< Data payload */
};

/**
 * @brief Initialize the MCP2515 CAN controller.
 * 
 * This function initializes the SPI communication with the MCP2515 and
 * configures it for J1939 communication (250kbps, 29-bit identifiers).
 * 
 * @return true if initialization is successful, false otherwise.
 */
bool mcp2515_init();

/**
 * @brief Transmit a CAN frame.
 * 
 * This function sends a CAN frame to the CAN bus via the MCP2515.
 * 
 * @param frame A pointer to the can_frame structure to be transmitted.
 * @return true if the frame is successfully queued for transmission, false otherwise.
 */
bool mcp2515_transmit(const can_frame* frame);

/**
 * @brief Receive a CAN frame.
 * 
 * This function checks for and receives a CAN frame from the MCP2515.
 * 
 * @param frame A pointer to a can_frame structure where the received frame will be stored.
 * @return true if a frame is successfully received, false if no frame is available.
 */
bool mcp2515_receive(can_frame* frame);
```

### 4.2. CLI

**`cli_command.h`**
```cpp
/**
 * @brief Struct defining the interface for any CLI command.
 * 
 * This structure defines the name, help text, and execute function for a CLI command.
 * It allows for a modular and extensible command system.
 */
struct CliCommand {
    const char* name;        /**< Command name (e.g., "set") */
    const char* help_text;   /**< A brief description of the command */
    void (*execute)(const std::vector<std::string>& args); /**< Function pointer to the implementation */
};
```

**`cli_handler.h`**
```cpp
/**
 * @brief Enum for the state of the CLI handler.
 */
enum CliState {
    CLI_STATE_NORMAL,    /**< Normal operation, waiting for commands */
    CLI_STATE_UPLOADING  /**< Waiting for file content to be uploaded */
};

/**
 * @brief Initializes and creates the CLI handler task.
 * 
 * This function initializes the CLI handler and creates a new FreeRTOS task
 * to handle the command line interface.
 */
void cli_handler_init();

/**
 * @brief Parses and executes a single command line string.
 * 
 * @param command The command line string to execute.
 */
void execute_command(char* command);

/**
 * @brief Enters the file upload mode.
 * 
 * @param filepath The path of the file to upload.
 */
void cli_enter_upload_mode(const std::string& filepath);
```

**`cli_output.h`**
```cpp
/**
 * @brief Sets the current output stream for all CLI commands.
 * 
 * This allows the output of the CLI to be redirected to different streams,
 * such as the Serial port or a Telnet client.
 * 
 * @param stream A pointer to the stream to use for output (e.g., &Serial, &wifiClient).
 */
void cli_set_output_stream(Stream* stream);

/**
 * @brief Gets the current output stream.
 * 
 * @return A pointer to the current output stream.
 */
Stream* cli_get_output_stream();

/**
 * @brief A printf-style function that directs its output to the current CLI stream.
 * 
 * @param format The format string.
 * @param ... The variable arguments.
 */
void cli_printf(const char* format, ...);
```

**`cli_registry.h`**
```cpp
/**
 * @brief Registers a new command with the CLI system.
 * 
 * @param command A pointer to the command struct to register.
 */
void cli_register_command(const CliCommand* command);

/**
 * @brief Finds a registered command by its name.
 * 
 * @param name The name of the command to find.
 * @return A pointer to the command struct, or nullptr if not found.
 */
const CliCommand* cli_find_command(const std::string& name);

/**
 * @brief Executes the main help command, printing help text for all registered commands.
 */
void cli_print_full_help();

/**
 * @brief Initializes and registers all CLI commands.
 */
void cli_register_all_commands();
```

**`dev_license_generator.h`**
```cpp
/**
 * @brief Generates a license key for testing purposes and prints it to the console.
 * 
 * @param uses The number of uses to encode in the key.
 * @param features The feature bitmask to encode.
 */
void generate_license_key_for_cli(int uses, int features);

/**
 * @brief Generates and activates a developer license with unlimited uses and all features.
 */
void activate_dev_license();
```

### 4.3. Core

**`error_handler.h`**
```cpp
/**
 * @brief Enum for the severity level of an error or event.
 */
enum class ErrorLevel {
    INFO,
    WARN,
    ERROR,
    CRITICAL
};

/**
 * @brief Reports an error or event. 
 *        This function formats the data into a JSON payload and sends it to the comms task
 *        for immediate MQTT publication. It also prints the error to the Serial console.
 * 
 * @param level The severity level of the event.
 * @param module The name of the module where the event occurred (e.g., "J1939", "UI").
 * @param message The specific error or event message.
 */
void error_report(ErrorLevel level, const char* module, const char* message);
```

**`filesystem_handler.h`**
```cpp
/**
 * @brief Initializes LittleFS.
 * 
 * This function initializes the LittleFS filesystem.
 * 
 * @return true if initialization is successful, false otherwise.
 */
bool filesystem_init();


/**
 * @brief Formats the filesystem and creates default files.
 * 
 * This function formats the LittleFS filesystem and creates the default
 * configuration files.
 * 
 * @return true if the format and creation is successful, false otherwise.
 */
bool filesystem_format_and_create_defaults();
```

**`shared_resources.h`**
```cpp
/**
 * @brief Mutex for protecting access to the global AppConfig struct.
 */
extern SemaphoreHandle_t config_mutex;

/**
 * @brief Mutex for protecting access to the global vehicle database.
 */
extern SemaphoreHandle_t vehicle_db_mutex;

/**
 * @brief Initializes all shared resources, such as mutexes.
 */
void shared_resources_init();
```

### 4.4. GPS

**`gps_handler.h`**
```cpp
/**
 * @brief Enum to represent GPS fix status.
 */
enum GpsFixStatus {
    GPS_FIX_NONE,
    GPS_FIX_2D,
    GPS_FIX_3D
};

/**
 * @brief Struct to hold parsed GPS data.
 */
struct GpsData {
    float latitude = 0.0;
    float longitude = 0.0;
    float speed_kmh = 0.0;
    bool is_valid = false;
    int year = 0;
    int month = 0;
    int day = 0;
    int hour = 0;
    int minute = 0;
    int second = 0;
    GpsFixStatus fix_status = GPS_FIX_NONE;
    int satellites = 0;
};

/**
 * @brief Queue for the GPS task to send parsed data to other tasks.
 */
extern QueueHandle_t gps_data_queue;

/**
 * @brief Initializes the GPS handler and creates the task.
 */
void gps_handler_init();

/**
 * @brief Gets the last known GPS data in a thread-safe way.
 * 
 * @param data Pointer to a GpsData struct to be filled.
 * @return True if the copied data is valid, false otherwise.
 */
bool gps_get_last_data(GpsData* data);

/**
 * @brief Gets the last known GPS time and populates a tm struct.
 * 
 * @param timeinfo Pointer to a tm struct to be filled.
 * @return True if the time is valid, false otherwise.
 */
bool gps_get_time(struct tm* timeinfo);

/**
 * @brief Gets the current GPS status.
 * 
 * @return The current GpsFixStatus.
 */
GpsFixStatus gps_get_status();
```

### 4.5. Hardware

**`hardware_handler.h`**
```cpp
/**
 * @brief Initializes the hardware handler.
 * 
 * This function initializes any hardware-specific components.
 */
void hardware_handler_init();

/**
 * @brief The main loop for the hardware handler.
 * 
 * This function is called repeatedly to handle any hardware-specific tasks.
 */
void hardware_handler_loop();
```

### 4.6. J1939

**`dm_handler.h`**
```cpp
/**
 * @brief Represents a single J1939 Diagnostic Trouble Code (DTC).
 */
struct DTC {
    uint32_t spn; /**< Suspect Parameter Number (19 bits) */
    uint8_t fmi;  /**< Failure Mode Identifier (5 bits) */
    uint8_t oc;   /**< Occurrence Count (7 bits) */
    uint8_t cm;   /**< SPN Conversion Method (1 bit) */
};

/**
 * @brief Parses a raw DM1 message payload (PGN 65226).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm1(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM2 message payload (PGN 65227).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm2(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM24 message payload (PGN 65238).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm24(const uint8_t* data, uint16_t length);

/**
 * @brief Represents a Freeze Frame of SPN data associated with a DTC.
 */
struct FreezeFrame {
    DTC trigger_dtc;
    std::map<uint32_t, float> spn_values;
};

/**
 * @brief Parses a raw DM4 message payload (PGN 65229).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm4(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM25 message payload (PGN 64951).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm25(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM11 message payload (PGN 65235).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm11(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM13 message payload (PGN 65237).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm13(const uint8_t* data, uint16_t length);

/**
 * @brief Parses a raw DM3 message payload (PGN 65228).
 * 
 * @param data Pointer to the raw byte array of the message.
 * @param length The length of the data array.
 */
void dm_parse_dm3(const uint8_t* data, uint16_t length);
```

**`fmi_db_handler.h`**
```cpp
/**
 * @brief Initializes the FMI description database by loading it from LittleFS.
 * 
 * @return True if the database was loaded successfully, false otherwise.
 */
bool fmi_db_init();

/**
 * @brief Gets the description for a given Failure Mode Identifier (FMI).
 * 
 * @param fmi The FMI to look up.
 * @return A const char* to the description, or "Unknown FMI" if not found.
 */
const char* fmi_get_description(uint8_t fmi);
```

**`j1939_handler.h`**
```cpp
/**
 * @brief Enum for the state of the J1939 Address Claiming process.
 */
enum J1939_ADDRESS_CLAIM_STATE {
    AC_STATE_UNCLAIMED,
    AC_STATE_CLAIMING,
    AC_STATE_CLAIMED,
    AC_STATE_CANNOT_CLAIM
};

/**
 * @brief Struct for a J1939 transmit request.
 */
struct J1939TxRequest {
    uint32_t pgn;
    uint8_t dest_address;
};

/**
 * @brief Queue for J1939 transmit requests.
 */
extern QueueHandle_t j1939_tx_queue;

/**
 * @brief Queue for received J1939 messages.
 */
extern QueueHandle_t j1939_rx_queue;

/**
 * @brief Initializes the J1939 handler and creates the task.
 */
void j1939_handler_init();

/**
 * @brief Requests a PGN from a specific destination address.
 * 
 * @param pgn The PGN to request.
 * @param dest_address The destination address.
 */
void j1939_request_pgn(uint32_t pgn, uint8_t dest_address);


/**
 * @brief Gets the current, claimed J1939 source address of this device.
 * 
 * @return The current source address (0-253), or 254 if unable to claim an address.
 */
uint8_t j1939_get_source_address();

/**
 * @brief Gets the timestamp of the last received J1939 message.
 * 
 * @return The timestamp of the last received message, in milliseconds.
 */
unsigned long j1939_get_last_rx_time();
```

**`j1939_pgn_definitions.h`**
```cpp
/**
 * @brief Maps a PGN to the list of SPNs it contains.
 */
static std::map<uint32_t, std::vector<uint32_t>> pgn_to_spns_map = {
    {61444, {190, 92}}, // EEC1: Engine Speed, Percent Load
    {65262, {110, 175}}, // ET1: Engine Coolant Temp, Engine Oil Temp
    {65263, {100}},      // EFP: Engine Oil Pressure
    {65265, {84, 91, 521}}, // CCVS1: Vehicle Speed, Accel Pedal Pos, Brake Pedal Pos
    {65266, {183}},      // LFE: Engine Fuel Rate
    {65270, {102, 105}}, // IC1: Intake Manifold Pressure & Temp
    {65271, {168}},      // VD: Vehicle Voltage
    {65276, {96}},        // LFC1: Fuel Level
    {65235, {}},          // DM11: Memory access response
    {65237, {}},           // DM13: Stop/start broadcast
    {65228, {}}           // DM3: Clear Previously Active DTCs
};

/**
 * @brief Reverse map for convenience: maps an SPN to its parent PGN.
 */
static std::map<uint32_t, uint32_t> spn_to_pgn_map;

/**
 * @brief Initializes the reverse map. Should be called once on startup.
 */
static inline void init_spn_to_pgn_map() {
    if (spn_to_pgn_map.empty()) {
        for (const auto& pair : pgn_to_spns_map) {
            for (uint32_t spn : pair.second) {
                spn_to_pgn_map[spn] = pair.first;
            }
        }
    }
}
```

**`j1939_pgn.h`**
```cpp
/**
 * @brief PGN for requesting another PGN
 */
#define J1939_PGN_REQUEST 59904 // 0xEA00

/**
 * @brief PGN for Address Claiming
 */
#define J1939_PGN_ADDRESS_CLAIM 60928 // 0xEE00

/**
 * @brief PGN for Engine Hours, Revolutions
 */
#define J1939_PGN_ENGINE_HOURS 65253 // 0xFEE5

/**
 * @brief PGN for Vehicle Speed
 */
#define J1939_PGN_VEHICLE_SPEED 65265 // 0xFEF1

/**
 * @brief PGN for Fuel Consumption
 */
#define J1939_PGN_FUEL_CONSUMPTION 65257 // 0xFEE9

/**
 * @brief PGN for Engine Temperature
 */
#define J1939_PGN_ENGINE_TEMPERATURE 65262 // 0xFEEE

/**
 * @brief PGN for Engine Oil Pressure
 */
#define J1939_PGN_ENGINE_OIL_PRESSURE 65263 // 0xFEEF

/**
 * @brief PGN for DM1 - Active DTCs
 */
#define J1939_PGN_DM1 65226 // 0xFECA

/**
 * @brief PGN for DM2 - Previously Active DTCs
 */
#define J1939_PGN_DM2 65227 // 0xFECB
```

**`j1939_service.h`**
```cpp
class J1939Service {
public:
    /**
     * @brief Gets the singleton instance of the J1939Service.
     */
    static J1939Service& getInstance() {
        static J1939Service instance;
        return instance;
    }

    /**
     * @brief Initializes the J1939 service and all related handlers.
     */
    void init();

    /**
     * @brief Starts the J1939 service.
     */
    void start();

    /**
     * @brief Stops the J1939 service.
     */
    void stop();

    // Add methods for sending/receiving messages, etc.

private:
    // Private constructor for singleton pattern
    J1939Service() {}

    // Delete copy constructor and assignment operator
    J1939Service(J1939Service const&) = delete;
    void operator=(J1939Service const&) = delete;
};
```

**`pdu_processor.h`**
```cpp
/**
 * @brief Initializes and creates the PDU processor task.
 * 
 * This task is responsible for processing received J1939 PDUs (Protocol Data Units),
 * which are complete messages, potentially reassembled from multiple CAN frames by the
 * transport protocol handler.
 */
void pdu_processor_init();
```

**`spn_db_handler.h`**
```cpp
/**
 * @brief Initializes the SPN description database by loading it from LittleFS.
 * 
 * @return True if the database was loaded successfully, false otherwise.
 */
bool spn_db_init();

/**
 * @brief Gets the description for a given Suspect Parameter Number (SPN).
 * 
 * @param spn The SPN to look up.
 * @return A const char* to the description, or "Unknown SPN" if not found.
 */
const char* spn_get_description(uint32_t spn);
```

**`spn_parser.h`**
```cpp
/**
 * @brief Struct to hold a parsed SPN value and its units.
 */
struct ParsedSPN {
    bool is_valid = false;
    float value;
    std::string units;
};

/**
 * @brief Parses a specific SPN from a given CAN frame.
 * 
 * @param frame The CAN frame containing the PGN that holds the SPN.
 * @param spn The SPN to parse.
 * @return A ParsedSPN struct with the value and units.
 */
ParsedSPN spn_parse(const can_frame& frame, uint32_t spn);
```

**`tp_handler.h`**
```cpp
/**
 * @brief Represents a fully reassembled multi-packet message.
 */
struct ReassembledPDU {
    uint32_t pgn;                  // PGN of the message
    uint8_t src_addr;              // Source address
    uint8_t data[MAX_TP_MESSAGE_SIZE];
    uint16_t data_length;
};

/**
 * @brief Queue for the TP handler to send fully reassembled PDUs to the application layer.
 */
extern QueueHandle_t j1939_complete_pdu_queue;

/**
 * @brief Struct for an outgoing multi-packet message request.
 */
struct OutgoingTPMessage {
    uint32_t pgn;
    uint8_t dest_addr;
    uint8_t data[MAX_TP_MESSAGE_SIZE];
    uint16_t data_length;
};

/**
 * @brief Queue for sending multi-packet messages.
 */
extern QueueHandle_t j1939_tx_tp_queue;

/**
 * @brief Initializes the Transport Protocol handler.
 */
void tp_handler_init();

/**
 * @brief Queues a multi-packet message to be sent via Transport Protocol.
 * 
 * @param pgn The PGN of the message to send.
 * @param dest_addr The destination address.
 * @param data A pointer to the data payload.
 * @param length The length of the data payload.
 */
void tp_send_message(uint32_t pgn, uint8_t dest_addr, const uint8_t* data, uint16_t length);

/**
 * @brief Processes an incoming CAN frame to check if it's part of a transport protocol session.
 * 
 * @param frame The incoming CAN frame.
 * @return True if the frame was consumed by the TP handler, false otherwise.
 */
bool tp_handler_process_frame(const can_frame* frame);
```

**`vehicle_db_handler.h`**
```cpp
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
```

### 4.7. License

**`license_handler.h`**
```cpp
/**
 * @brief Feature flags that can be licensed.
 */
enum class LicensedFeature : uint8_t {
    TELEMETRY = 1 << 0, /**< Bit 0 */
    J1939_PRO = 1 << 1, /**< Bit 1 (Example for a future feature) */
    // Add up to 8 features here
};

/**
 * @brief Initializes the licensing system.
 */
void license_handler_init();

/**
 * @brief Gets the unique hardware ID of the device.
 * 
 * @return A string containing the hardware ID.
 */
String license_get_hw_id();

/**
 * @brief Activates a new license key.
 * 
 * @param key The 12-character license key to activate.
 * @return True if the license key is valid and was activated successfully, false otherwise.
 */
bool license_activate(const char* key);

/**
 * @brief Gets the number of remaining uses for the current license.
 * 
 * @return The number of uses left. Returns 0 if no valid license is active.
 */
int license_get_remaining_uses();

/**
 * @brief Checks if a specific feature is enabled by the current license.
 * 
 * @param feature The feature to check.
 * @return True if the feature is licensed and there are uses remaining, false otherwise.
 */
bool license_is_feature_active(LicensedFeature feature);

/**
 * @brief Consumes one use from the current license. 
 *        This should be called when a licensed feature is used.
 */
void license_consume_use();
```

### 4.8. OTA

**`ota_handler.h`**
```cpp
/**
 * @brief Initializes the OTA handler.
 * 
 * This function initializes the OTA update mechanism.
 */
void ota_handler_init();

/**
 * @brief The main loop for the OTA handler.
 * 
 * This function is called repeatedly to handle OTA updates.
 */
void ota_handler_loop();
```

### 4.9. Settings

**`config.h`**
```cpp
#define FIRMWARE_VERSION "4.3.0"

/**
 * @brief Struct to hold all application configuration.
 */
struct AppConfig {
    uint8_t j1939_node_address;
    uint64_t j1939_name;
    struct {
        String ssid;
        String password;
    } wifi;
    struct {
        String broker;
        int port;
        String topic;
    } mqtt;
    struct {
        uint8_t select_pin;
        uint8_t next_pin;
    } buttons;
    struct {
        int spi_host;
        int sclk_pin;
        int mosi_pin;
        int cs_pin;
        int dc_pin;
        int rst_pin;
        int bl_pin;
        uint8_t display_brightness;
    } display;
    struct {
        int spi_host;
        int sclk_pin;
        int mosi_pin;
        int miso_pin;
        int cs_pin;
    } mcp2515;
    struct {
        String hostname;
        String password;
    } ota;
    struct {
        bool wifi_enabled;
        bool mqtt_enabled;
        bool gps_enabled;
    } features;
};

/**
 * @brief Global instance of the application configuration.
 * 
 * This will be populated by the filesystem_handler on startup.
 */
extern AppConfig config;
```

**`settings_handler.h`**
```cpp
/**
 * @brief Initializes the NVS (Preferences) system.
 */
void settings_init();

/**
 * @brief Loads settings from NVS and overrides the default config.
 */
void settings_load();

/**
 * @brief Saves the current in-memory config to NVS.
 */
void settings_save();

/**
 * @brief Checks for a firmware version change and handles necessary updates (e.g., resetting flags).
 */
void settings_check_version_update();

/**
 * @brief Checks if the developer command has been used for the current firmware version.
 * 
 * @return True if the command is disabled, false otherwise.
 */
bool settings_is_dev_cmd_disabled();

/**
 * @brief Disables the developer command for the current firmware version.
 */
void settings_disable_dev_cmd();
```

### 4.10. Time

**`time_handler.h`**
```cpp
/**
 * @brief Enum to define the source for time synchronization.
 */
enum TimeSource {
    TIME_SOURCE_NTP,
    TIME_SOURCE_GPS
};

/**
 * @brief Initializes the time service.
 * 
 * @param source The initial time source to use (NTP or GPS).
 */
void time_handler_init(TimeSource source = TIME_SOURCE_NTP);

/**
 * @brief Sets the time synchronization source.
 * 
 * @param source The time source to use (NTP or GPS).
 */
void time_set_source(TimeSource source);

/**
 * @brief Synchronizes the system time from the configured source.
 * 
 * @return True if synchronization was successful, false otherwise.
 */
bool time_sync();

/**
 * @brief Gets the current time as a formatted ISO8601 string.
 * 
 * @return A string with the current timestamp (e.g., "2025-10-11T18:30:00Z"),
 *         or "[TIME NOT SYNCED]" if the time has not been synchronized yet.
 */
String time_handler_get_timestamp();
```

### 4.11. UI

**`button_driver.h`**
```cpp
/**
 * @brief Enum for all possible button events.
 */
typedef enum {
    BTN_EVENT_NONE,
    // Select Button Events
    BTN_PRESS_SELECT,
    BTN_LONG_PRESS_SELECT,
    BTN_DOUBLE_CLICK_SELECT,
    // Next Button Events
    BTN_PRESS_NEXT,
    BTN_LONG_PRESS_NEXT,
    BTN_DOUBLE_CLICK_NEXT,
} button_event_t;

/**
 * @brief Queue for the button driver to send events to the UI task.
 */
extern QueueHandle_t button_event_queue;


/**
 * @brief Initializes the button driver and creates the polling task.
 */
void button_driver_init();
```

**`simple_font.h`**
```cpp
/**
 * @brief Basic 5x7 font for ASCII characters 32-127.
 */
static const uint8_t font[96][5] = { ... };
```

**`st7789_driver.h`**
```cpp
/**
 * @brief Basic 16-bit color definitions (RGB565).
 */
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFDA0

/**
 * @brief Initialize the display.
 */
void st7789_init();

/**
 * @brief Fill the screen with a color.
 * 
 * @param color The 16-bit color to fill the screen with.
 */
void st7789_fill_screen(uint16_t color);

/**
 * @brief Draw a single line of text.
 * 
 * @note This is a very basic implementation with a fixed font.
 * 
 * @param text The text to draw.
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param color The 16-bit color of the text.
 * @param bg_color The 16-bit color of the background.
 */
void st7789_draw_text(const char* text, int16_t x, int16_t y, uint16_t color, uint16_t bg_color);

/**
 * @brief Draw a filled rectangle.
 * 
 * @param x The x-coordinate of the top-left corner.
 * @param y The y-coordinate of the top-left corner.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The 16-bit color to fill the rectangle with.
 */
void st7789_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);


/**
 * @brief Set the display brightness.
 * 
 * @param value The brightness value (0-255).
 */
void st7789_set_brightness(uint8_t value);
```

**`ui_manager.h`**
```cpp
class UIManager {
public:
    /**
     * @brief Gets the singleton instance of the UIManager.
     */
    static UIManager& getInstance();

    /**
     * @brief Initializes the UI manager.
     */
    void init();

    /**
     * @brief Runs the UI loop (handles input and drawing).
     */
    void run();

    /**
     * @brief Pushes a new screen onto the screen stack.
     * @param screen A shared pointer to the screen to push.
     */
    void push_screen(std::shared_ptr<Screen> screen);

    /**
     * @brief Pops the current screen from the screen stack.
     */
    void pop_screen();
};
```

**`menu.h`**
```cpp
class Menu {
public:
    Menu(const std::vector<std::string>& items);

    void handle_input();
    void draw();

    int get_selected_item() const;
};
```

**`title_bar.h`**
```cpp
class TitleBar {
public:
    void set_title(const std::string& title);
    void draw();
};
```

### 4.12. WiFi

**`comms_handler.h`**
```cpp
/**
 * @brief Enum for the state of the Communications Handler state machine.
 */
enum CommsState {
    COMMS_STATE_STARTING,
    COMMS_STATE_WIFI_CONNECTING,
    COMMS_STATE_MQTT_CONNECTING,
    COMMS_STATE_OPERATIONAL,
    COMMS_STATE_WIFI_DISCONNECTED
};

/**
 * @brief Enum for the detailed WiFi status.
 */
enum WiFiStatus {
    WIFI_STATUS_DISCONNECTED,
    WIFI_STATUS_CONNECTING,
    WIFI_STATUS_CONNECTED,
    WIFI_STATUS_INTERNET
};

/**
 * @brief Queue for high-priority telemetry messages (payloads are heap-allocated strings).
 */
extern QueueHandle_t telemetry_queue;

/**
 * @brief Initializes the Communications handler and creates the task.
 */
void comms_handler_init();

/**
 * @brief Sends a JSON payload to the telemetry queue for immediate publication.
 * 
 * @param payload A heap-allocated C-string (char*) containing the JSON payload.
 *                The comms task will take ownership and free the memory.
 * @return True if the message was successfully sent to the queue.
 */
bool comms_publish_telemetry(const char* payload);

/**
 * @brief Gets the current WiFi status.
 * 
 * @return The current WiFiStatus.
 */
WiFiStatus comms_get_wifi_status();
```