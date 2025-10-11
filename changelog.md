# Histórico de Alterações do Projeto

### v3.3.0 (2025-10-11)
- **Base de Dados de Veículos e Centralinas:**
  - Implementada a fundação para um sistema de diagnóstico baseado em modelos de veículos.
  - Criado um ficheiro `vehicles.json` na pasta `data/` para armazenar uma base de dados de modelos de autocarros (diesel e elétricos) e as suas respetivas centralinas (ECUs).
  - Desenvolvido um novo módulo `vehicle_db_handler` para carregar, interpretar e guardar a base de dados a partir do LittleFS.
  - Adicionados novos comandos à CLI para interagir com a base de dados:
    - `list_models`: Lista todos os modelos de veículos disponíveis.
    - `show_model <name>`: Mostra os detalhes de um modelo específico, incluindo as suas ECUs e PGNs de interesse.
    - `save_db`: Guarda para o ficheiro `vehicles.json` quaisquer alterações feitas em memória (preparação para futuras funcionalidades de edição).

### v3.2.0 (2025-10-11)
- **Leitura de VIN (Número de Identificação do Veículo):**
  - Implementada a descodificação do PGN 65260 para extrair o VIN do veículo.
  - O `pdu_processor` agora interpreta a mensagem de VIN recebida através do protocolo de transporte.
  - Adicionado um novo comando `request_vin` à CLI para solicitar ativamente o VIN à rede.

### v3.1.0 (2025-10-11)
- **Implementação do J1939 Address Claiming:**
  - O `j1939_handler` foi refatorado para implementar a máquina de estados de reivindicação de endereço, tornando o dispositivo compatível com o standard J1939.
  - No arranque, o dispositivo agora reivindica o seu endereço preferido e gere conflitos com outros dispositivos na rede, comparando o seu NAME de 64 bits.
  - Em caso de conflito perdido, o dispositivo adota o endereço NULO (254) e opera em modo de escuta.
  - O NAME J1939 do dispositivo é agora configurável através do `config.json`.

### v3.0.0 (2025-10-11)
- **Implementação do Protocolo de Transporte J1939 (TP):**
  - Criado um novo módulo `tp_handler` para gerir mensagens J1939 multi-pacote.
  - Implementado o suporte para a remontagem de mensagens broadcast (TP.CM BAM).
  - O `j1939_handler` foi atualizado para passar frames de transporte para o novo módulo.
  - Adicionada uma nova tarefa `pdu_processor` que recebe as mensagens completas e remontadas, preparando o sistema para a interpretação de mensagens de diagnóstico (DM).

### v2.9.0 (2025-10-11)
- **Refatoração da Arquitetura da CLI:**
  - O sistema de CLI foi completamente reescrito para uma arquitetura modular e extensível, baseada no "Command Pattern".
  - A lógica de cada comando foi movida para um ficheiro individual em `src/cli/commands/`.
  - Criado um registo central (`cli_registry`) para a descoberta e execução de comandos, eliminando a necessidade de um bloco `if/else if` monolítico.
  - Adicionar novos comandos ao sistema tornou-se mais simples e seguro.
  - A experiência de utilização da CLI foi melhorada com a adição de eco de caracteres e suporte para backspace.

### v2.8.0 (2025-10-11)
- **Licença de Desenvolvimento e Controlo de Versão:**
  - Adicionado um comando secreto `_dev_license` para ativar uma licença de desenvolvimento com usos ilimitados.
  - Este comando só pode ser utilizado uma vez por cada versão de firmware.
  - Implementado um sistema de verificação de versão no arranque: ao detetar uma nova versão de firmware, o sistema reativa o comando `_dev_license` para essa versão.

### v2.7.0 (2025-10-11)
- **Sistema de Licenciamento Avançado:**
  - O `license_handler` foi completamente redesenhado para um sistema de licenciamento de nível profissional.
  - As chaves de licença são agora códigos de 12 caracteres que contêm, de forma ofuscada e otimizada (bit-packing), o número de usos, flags de funcionalidades, e uma data de emissão.
  - **Segurança da Chave:**
    - A chave é de uso único e não é armazenada no dispositivo após a ativação.
    - A validação é feita com uma assinatura HMAC-SHA256 truncada para garantir a autenticidade e a ligação ao ID de hardware do dispositivo.
  - **Restrições Temporais:**
    - A ativação da licença tem de ser feita num prazo de 7 dias após a sua data de emissão.
    - O dispositivo tem de ter a hora do sistema sincronizada via NTP para permitir a ativação.
  - **Proteção Anti-Brute-Force:** O sistema bloqueia novas tentativas de ativação por 15 minutos após 5 tentativas falhadas.
  - As funcionalidades licenciadas (ex: Telemetria) são agora verificadas com flags de bits, permitindo um controlo granular.
  - O comando de CLI `_genkey` foi atualizado para gerar estas novas chaves complexas para fins de teste.

### v2.5.0 (2025-10-11)
- **Gestão de Funcionalidades e Sincronização de Tempo:**
  - Adicionada a capacidade de ativar ou desativar serviços individuais (Wi-Fi, MQTT, GPS) através de flags de configuração (`f_wifi`, `f_mqtt`, `f_gps`) na CLI.
  - A lógica de arranque foi refatorada para inicializar os módulos apenas se as suas respetivas flags de funcionalidade estiverem ativas.
  - Implementado um novo módulo `time_handler` para sincronizar a hora do sistema com um servidor NTP quando o Wi-Fi está ativo.
  - Todos os relatórios de eventos e publicações de dados via MQTT agora usam um timestamp no formato ISO8601 (UTC), em vez de `millis()`, garantindo uma cronologia correta dos dados.

### v2.4.0 (2025-10-11)
- **Gestão de Configurações via CLI e NVS:**
  - Adicionado um novo módulo `settings_handler` para gerir o armazenamento de configurações na NVS (Non-Volatile Storage) do ESP32, usando a biblioteca `Preferences`.
  - A aplicação agora carrega as configurações por defeito do `config.json` e de seguida sobrepõe com as configurações guardadas na NVS.
  - Implementados novos comandos na CLI:
    - `set <key> <value>`: para alterar uma configuração em memória.
    - `save`: para persistir todas as as configurações atuais na NVS.
  - A CLI agora suporta a configuração de `wifi_ssid`, `wifi_pass`, `mqtt_broker`, `mqtt_port`, `mqtt_topic`, `ota_host` e `ota_pass`.
  - As senhas foram removidas do ficheiro `config.json` por defeito para aumentar a segurança.

### v2.3.0 (2025-10-11)
- **Telemetria Avançada e Gestão de Erros:**
  - Criado um novo módulo `error_handler` para gestão centralizada de eventos e erros.
  - Implementado um sistema de reporte de eventos (INFO, WARN, ERROR) via MQTT.
  - Os relatórios de erro incluem dados de localização GPS para análise geo-referenciada.
  - O `filesystem_handler` e o `comms_handler` foram integrados com o novo sistema de erros.
- **Atualizações Over-the-Air (OTA):**
  - Adicionada a capacidade de atualização remota do firmware via Wi-Fi (OTA).
  - A configuração do OTA (hostname, password) é gerida através do `config.json`.
  - O estado do processo de OTA é reportado via MQTT, usando o novo `error_handler`.
- **Melhorias de Robustez e Arquitetura:**
  - O `gps_handler` foi tornado thread-safe através do uso de um mutex para aceder aos últimos dados conhecidos.
  - Criada uma fila de mensagens de alta prioridade (`telemetry_queue`) no `comms_handler` para garantir a publicação imediata de eventos críticos.
  - A sequência de arranque foi otimizada para garantir que os serviços de comunicação estão ativos antes de operações críticas do sistema de ficheiros.

### v2.2.1 (2025-10-10)
- **Reorganização de Módulos:**
  - O driver do ecrã (`st7789_driver`, `simple_font`) foi movido de `src/ui` para `src/driver/display/`.
  - O módulo de GPS (`gps_handler`) foi movido de `src/gps` para `src/comms/gps/` para agrupar todos os módulos de comunicação.
  - Os caminhos de inclusão (`#include`) foram atualizados em todos os ficheiros afetados pela reorganização.

### v2.2.0 (2025-10-10)
- **Adicionadas Funcionalidades de Conectividade e GPS:**
  - Adicionado um novo módulo `gps_handler` que corre numa tarefa FreeRTOS dedicada para ler e processar dados NMEA de um módulo GPS via UART.
  - Adicionado um novo módulo `comms_handler` para gestão de comunicações de rede.
  - Implementada a ligação a redes WiFi usando as credenciais do ficheiro de configuração.
  - Implementada a publicação de dados para um broker MQTT.
  - Os dados do J1939 e do GPS são agregados, formatados em JSON e publicados via MQTT.
  - O ficheiro `config.json` foi atualizado para incluir secções de `wifi` e `mqtt`.

### v2.1.0 (2025-10-10)
- **Driver de Botões Avançado:**
  - Criado um novo módulo `button_driver` com uma máquina de estados para detetar `single press`, `long press` e `double click`.

### v2.0.0 (2025-10-10)
- **Refatoração Completa para FreeRTOS:**
  - O projeto foi reescrito para uma arquitetura multi-tarefa com FreeRTOS e configuração via LittleFS.

### v1.3.0 (2025-10-10)
- Adicionado Sistema de Menus e Interface de Linha de Comandos (CLI).

### v1.2.0 (2025-10-10)
- Reestruturação profunda dos ficheiros em subpastas.

### v1.1.0 (2025-10-10)
- Adaptação de Hardware para a placa LilyGo T-Display S3 e módulo CAN MCP2515.

### v1.0.0 (2025-10-10)
- Inicialização do Projeto com stack J1939 base.
