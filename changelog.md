# Histórico de Alterações do Projeto

### v3.23.0 (2025-10-11)
- **Diagnóstico Avançado com DM4 (Freeze Frame):**
  - Implementada a capacidade de pedir e descodificar mensagens DM4 (Freeze Frame).
  - Adicionado o comando `request_dm4 <address>` à CLI para pedir a uma ECU o "instantâneo" dos dados no momento de uma falha.
  - O `dm_handler` agora inclui uma implementação inicial para interpretar a estrutura das mensagens DM4.

### v3.22.0 (2025-10-11)
- **Expansão do Diagnóstico com DM24:**
  - Implementada a capacidade de pedir e descodificar mensagens DM24 (Supported DTCs).
  - Adicionado o comando `request_dm24 <address>` à CLI para pedir a uma ECU a lista de todos os DTCs que ela suporta.
  - A lista de DTCs suportados é agora impressa na consola, enriquecida com as descrições dos SPNs.

### v3.21.0 (2025-10-11)
- **Finalização da Ferramenta Gráfica (GUI):**
  - Implementada a lógica completa para upload e download de ficheiros no separador "File Manager" da aplicação `gui_tool.py`.
  - Adicionado um sistema de estados à GUI para gerir as transferências de ficheiros em segundo plano sem bloquear a interface.
  - Implementado o separador "Configuration" para ler e escrever configurações do dispositivo diretamente a partir da GUI.

### v3.20.0 (2025-10-11)
- **Criação de Ferramenta Gráfica (GUI) em Python:**
  - Desenvolvida uma nova aplicação de desktop (`gui_tool.py`) com `tkinter` para interagir com o dispositivo.
  - A GUI inclui uma interface com separadores para gestão da ligação série, um terminal, um gestor de ficheiros e um editor de configurações.
  - Adicionado o comando `get_config` ao firmware para permitir que a GUI leia a configuração atual do dispositivo.
  - A GUI integra a lógica de listagem, upload e download de ficheiros, e de leitura/escrita de configurações, comunicando através da porta série.

### v3.19.0 (2025-10-11)
- **Melhoria da Visualização de Dados em Tempo Real:**
  - A base de dados de SPNs foi significativamente expandida para incluir dezenas de parâmetros comuns.
  - O `spn_parser` foi atualizado para conseguir descodificar todos estes novos SPNs.
  - Criado um sistema de mapeamento estruturado entre PGNs e SPNs para uma associação de dados mais correta e escalável.
  - Os ecrãs de "Live Data" (genérico e por ECU) foram atualizados para usar este novo sistema, mostrando agora uma vasta gama de valores legíveis e formatados, tornando a ferramenta muito mais útil para o diagnóstico em tempo real.

### v3.18.0 (2025-10-11)
- **Ecrã de Live Data por ECU:**
  - Implementado um novo ecrã na UI para mostrar dados em tempo real de uma ECU específica, selecionada através da base de dados de veículos.
  - O comando `read_ecu` foi refatorado para ativar este novo ecrã e passar o contexto da ECU selecionada.
  - Ao entrar no ecrã, a UI agora envia pedidos para os PGNs de interesse definidos para essa ECU, permitindo uma monitorização focada e contextual.

### v3.17.0 (2025-10-11)
- **Melhoria na Descodificação de DTCs com Descrições de FMI:**
  - Criada uma base de dados (`fmi_descriptions.json`) para mapear os códigos FMI (Failure Mode Identifier) para as suas descrições standard J1939.
  - Adicionado um novo módulo `fmi_db_handler` para carregar e gerir esta base de dados.
  - O ecrã de visualização de DTCs foi atualizado para mostrar não só a descrição do SPN, mas também a descrição do FMI, tornando o diagnóstico muito mais intuitivo e completo.

### v3.16.0 (2025-10-11)
- **Visualização de Dados em Tempo Real no Ecrã:**
  - Criado um novo módulo `spn_parser` para descodificar valores de SPNs específicos (ex: RPM, temperatura) a partir dos seus dados brutos.
  - O ecrã "Live Data" foi completamente redesenhado para mostrar uma lista de parâmetros J1939 importantes com os seus valores físicos e unidades, em vez de apenas dados CAN brutos.
  - A interface de utilizador (`ui_handler`) foi refatorada para usar o novo parser e apresentar os dados de forma legível.

### v3.15.0 (2025-10-11)
- **Visualização de DTCs no Ecrã:**
  - Criada uma base de dados (`spn_descriptions.json`) para mapear SPNs para descrições legíveis.
  - O `dm_handler` foi melhorado para enriquecer os dados dos DTCs com as descrições da nova base de dados.
  - O `ui_handler` foi refatorado para incluir um novo ecrã dedicado à visualização de DTCs.
  - Os erros de diagnóstico (DM1/DM2) são agora enviados para a tarefa da UI e apresentados no ecrã com a sua descrição, permitindo um diagnóstico visual e em tempo real.

### v3.13.0 (2025-10-11)
- **Acesso Remoto via Wi-Fi:**
  - Implementado um servidor Telnet para acesso completo à CLI através da rede (porta 23).
  - Implementado um servidor HTTP para gestão de ficheiros (porta 80) com os seguintes endpoints:
    - `GET /files`: Lista os ficheiros no dispositivo.
    - `GET /download?path=<path>`: Faz o download de um ficheiro.
    - `POST /upload`: Faz o upload de um ficheiro.
  - O `comms_handler` foi refatorado para gerir os novos servidores (Telnet e HTTP).
  - O script `file_manager.py` foi atualizado para suportar tanto a comunicação via Série como via HTTP, permitindo a gestão de ficheiros através da rede.

### v3.12.0 (2025-10-11)
- **Acesso Remoto à CLI via Telnet:**
  - Implementado um servidor Telnet que permite o acesso à linha de comandos do dispositivo através da rede Wi-Fi (porta 23).
  - O sistema de output da CLI foi completamente refatorado para ser agnóstico ao meio de comunicação, permitindo que a resposta dos comandos seja direcionada tanto para a porta Série como para um cliente Telnet.
  - Todos os logs do sistema e a interação com os comandos estão agora disponíveis remotamente, facilitando o diagnóstico e a gestão do dispositivo sem necessidade de ligação física.

### v3.11.0 (2025-10-11)
- **Ferramentas Externas e Gestão de Ficheiros:**
  - Criado um novo script Python, `file_manager.py`, para automatizar o upload e download de ficheiros para o dispositivo através da porta série.
  - O script interage com os comandos `upload` e `download` da CLI, simplificando a gestão de ficheiros como `vehicles.json` e `config.json`.

### v3.10.0 (2025-10-11)
- **Transferência de Ficheiros via CLI:**
  - Implementada a capacidade de fazer upload e download de ficheiros para o sistema de ficheiros LittleFS do dispositivo.
  - Adicionado o comando `download <filepath>` para obter ficheiros (codificados em Base64).
  - Adicionado o comando `upload <filepath>` e um "modo de upload" para enviar ficheiros para o dispositivo.
  - O `cli_handler` foi refatorado para uma máquina de estados (`NORMAL`, `UPLOADING`) para gerir a transferência de ficheiros de forma robusta e com timeout.

### v3.9.0 (2025-10-11)
- **Edição da Base de Dados de Veículos via CLI:**
  - O `vehicle_db_handler` foi expandido com funções para modificar a base de dados em memória.
  - Adicionados novos comandos à CLI para uma gestão completa da base de dados de veículos:
    - `add_model <name> <type>`: Adiciona um novo modelo de veículo.
    - `add_ecu <model> <ecu> <addr>`: Adiciona uma nova ECU a um modelo.
    - `add_pgn <model> <ecu> <pgn>`: Adiciona um PGN de interesse a uma ECU.
  - As alterações podem ser persistidas no ficheiro `vehicles.json` usando o comando `save_db`.

### v3.8.0 (2025-10-11)
- **Gestão de Códigos de Erro (DTC):**
  - Adicionado o comando `clear_dtcs <address>` à CLI para enviar pedidos de limpeza de erros ativos (DM11) e passados (DM3) a uma ECU específica.
  - O `j1939_handler` foi melhorado para interpretar mensagens de Confirmação (PGN 59392, Acknowledgement).
  - O sistema agora reporta se um comando (como o de limpar DTCs) foi aceite (ACK Positivo) ou rejeitado (ACK Negativo) pela ECU, fornecendo feedback crucial ao utilizador.

### v3.7.0 (2025-10-11)
- **Leitura de Dados por Centralina (ECU):**
  - O `vehicle_db_handler` foi expandido para gerir um "contexto" de veículo ativo.
  - Adicionado o comando `select_model <modelo>` à CLI para definir o veículo em que se está a trabalhar.
  - Adicionado o comando `read_ecu <ecu>` à CLI. Este comando consulta a base de dados, encontra a ECU para o modelo de veículo ativo, e pede todos os PGNs de interesse definidos para essa ECU.

### v3.6.0 (2025-10-11)
- **Descodificação de Mensagens de Diagnóstico (DM1/DM2):**
  - Criado um novo módulo `dm_handler` para descodificar mensagens de diagnóstico.
  - Implementada a lógica de parsing para extrair os Códigos de Falha de Diagnóstico (DTCs) de mensagens DM1 (erros ativos) e DM2 (erros passados).
  - O `pdu_processor` agora utiliza o `dm_handler` para processar e reportar os DTCs recebidos.
  - Adicionados novos comandos à CLI:
    - `request_dm1`: Para solicitar os erros ativos.
    - `request_dm2`: Para solicitar os erros passados.

### v3.5.0 (2025-10-11)
- **Melhorias de Robustez e Concorrência:**
  - Adicionado um novo módulo `shared_resources` para gerir "locks" (mutexes) de forma centralizada.
  - O acesso aos objetos de dados globais (`config` e `vehicle_database`) foi protegido com mutexes para garantir a segurança em ambientes com múltiplas tarefas (thread-safety).
  - O comando `set` da CLI foi melhorado com tratamento de exceções (`try...catch`) para evitar falhas com inputs numéricos inválidos.
  - A estrutura do código foi melhorada movendo a lógica de implementação de comandos de desenvolvimento para ficheiros `.cpp`, seguindo as melhores práticas.

### v3.4.0 (2025-10-11)
- **Refatoração do Gestor de Comunicações (`comms_handler`):**
  - O `comms_handler` foi completamente reescrito para usar uma máquina de estados robusta, melhorando a gestão do ciclo de vida da ligação.
  - A nova arquitetura gere os estados de ligação ao Wi-Fi, sincronização NTP e ligação ao MQTT de forma mais resiliente e não-bloqueante.
  - A lógica de reconexão automática para o Wi-Fi e MQTT está agora integrada na máquina de estados, tornando o sistema mais estável em caso de falhas de rede.

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
