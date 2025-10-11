# Ferramenta de Diagnóstico J1939 para ESP32

**Versão Atual:** 2.4.0

## Sobre o Projeto

Esta é uma aplicação de diagnóstico J1939 de nível profissional para a plataforma ESP32. O projeto foi desenhado com foco em robustez, modularidade e performance, utilizando uma arquitetura multi-tarefa baseada em FreeRTOS para garantir uma operação concorrente e não-bloqueante.

A ferramenta permite a monitorização de redes J1939, o envio de pedidos de PGNs, e a telemetria de dados e eventos via MQTT. A configuração de parâmetros sensíveis é feita de forma segura através de uma interface de linha de comandos (CLI) e guardada na memória não-volátil (NVS) do dispositivo.

## Funcionalidades Chave

- **Arquitetura Multi-tarefa (FreeRTOS):** Cada subsistema (CAN, UI, CLI, Comunicações) corre de forma independente na sua própria tarefa, garantindo performance e responsividade.
- **Telemetria Avançada via MQTT:** Publica dados da rede J1939 e eventos do sistema (erros, status) para um broker MQTT, incluindo dados de geolocalização (GPS) nos relatórios de erro.
- **Configuração Segura via CLI:** As configurações de rede (Wi-Fi, MQTT) e de serviços (OTA) são geridas através de comandos na CLI e guardadas de forma segura na NVS, evitando a exposição de senhas em ficheiros de texto.
- **Atualizações Over-the-Air (OTA):** O firmware pode ser atualizado remotamente através da rede Wi-Fi, sem necessidade de acesso físico ao dispositivo.
- **Drivers Dedicados:** Drivers de baixo nível para o ecrã ST7789 e o controlador CAN MCP2515, escritos via SPI para máximo controlo e performance.
- **Sistema de Ficheiros:** Utiliza LittleFS para carregar configurações base e outros recursos.
- **Interface de Utilizador:** Inclui um sistema de menus interativo no ecrã e uma interface de linha de comandos completa via porta Serial.

## Hardware Necessário

- **Placa de Desenvolvimento:** [LilyGo T-Display S3](https://www.lilygo.cc/products/t-display-s3) (ESP32-S3)
- **Módulo CAN:** Um transceiver CAN baseado no chip **MCP2515**.
- **Módulo GPS:** [u-blox NEO-M8N](https://www.u-blox.com/en/product/neo-m8n-module) ou similar, com interface UART (opcional, para geolocalização).

## Software e Bibliotecas

O projeto é construído sobre o **Arduino Core para ESP32 (v3.3.0 ou superior)** e utiliza as seguintes bibliotecas principais:

- **FreeRTOS:** Integrado no core do ESP32, para gestão das tarefas.
- **LittleFS:** Para o sistema de ficheiros.
- **ArduinoJson:** Para parsing do ficheiro de configuração `config.json`.
- **PubSubClient:** Para a comunicação MQTT.
- **WiFi, ESPmDNS, ArduinoOTA:** Para conectividade Wi-Fi e atualizações OTA.
- **Preferences:** Para acesso à NVS (Non-Volatile Storage).

## Como Começar

### 1. Pré-requisitos

- É recomendado o uso de [PlatformIO](https://platformio.org/) para compilar e carregar o projeto.

### 2. Compilar e Carregar

- **Compilar:** `pio run`
- **Carregar para o dispositivo:** `pio run --target upload`

### 3. Configuração Inicial

Na primeira utilização, é necessário configurar as credenciais de rede.

1.  Ligue-se ao dispositivo através de um monitor Serial (baud rate: 115200).
2.  Use os comandos da CLI para configurar o Wi-Fi:
    ```bash
    set wifi_ssid "O_NOME_DA_SUA_REDE"
    set wifi_pass "A_SUA_SENHA_WIFI"
    ```
3.  Configure os detalhes do seu servidor MQTT e da segurança OTA:
    ```bash
    set mqtt_broker 192.168.1.100
    set ota_pass umasenhaparaota
    ```
4.  Guarde as configurações para que persistam após o reinício:
    ```bash
    save
    ```
5.  Reinicie o dispositivo.

## Utilização

Após a configuração, o dispositivo irá ligar-se à rede e começar a operar. Pode interagir com o mesmo através do sistema de menus no ecrã ou da interface de linha de comandos.

Para uma documentação detalhada de todos os comandos da CLI, consulte o ficheiro [CLI_COMMANDS.md](CLI_COMMANDS.md).

---

Para o histórico de alterações completo, consulte o ficheiro `changelog.md`.
