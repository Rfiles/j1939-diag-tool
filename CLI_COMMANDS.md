# Documentação da Interface de Linha de Comandos (CLI)

Esta é a documentação para a Interface de Linha de Comandos (CLI) da Ferramenta de Diagnóstico J1939 (Versão 3.3.0).

## Comandos de Configuração

### `set <key> <value>`
Define o valor de uma chave de configuração na memória. A alteração só se torna permanente após usar o comando `save`.

- **Sintaxe:** `set <key> <value>`
- **Exemplo:** `set wifi_ssid "A Minha Rede"`
- **Chaves Disponíveis:**
  - `wifi_ssid`, `wifi_pass`: Credenciais da rede Wi-Fi.
  - `mqtt_broker`, `mqtt_port`, `mqtt_topic`: Configurações do servidor MQTT.
  - `ota_host`, `ota_pass`: Nome e senha para atualizações OTA.
  - `f_wifi`, `f_mqtt`, `f_gps`: Ativa/desativa funcionalidades (`true` ou `false`).

### `save`
Guarda todas as configurações atuais para a memória persistente (NVS).

- **Sintaxe:** `save`

---

## Comandos de Diagnóstico J1939

### `request <pgn>`
Envia um pedido global para um PGN (Parameter Group Number) específico.

- **Sintaxe:** `request <pgn>`
- **Exemplo:** `request 65262`

### `request_vin`
Envia um pedido global para o PGN de Identificação do Veículo (VIN).

- **Sintaxe:** `request_vin`

---

## Comandos do Sistema de Licenciamento

### `get_hw_id`
Mostra o ID de Hardware único do dispositivo, necessário para gerar uma licença.

- **Sintaxe:** `get_hw_id`

### `activate <license_key>`
Ativa uma licença no dispositivo.

- **Sintaxe:** `activate <key>`
- **Exemplo:** `activate AbCd12EfGhIJ`

### `get_license_info`
Verifica o estado da licença atual, incluindo usos restantes e funcionalidades ativas.

- **Sintaxe:** `get_license_info`

---

## Comandos da Base de Dados de Veículos

### `list_models`
Lista todos os modelos de veículos disponíveis na base de dados `vehicles.json`.

- **Sintaxe:** `list_models`

### `show_model <model_name>`
Mostra os detalhes de um modelo de veículo específico, incluindo as suas ECUs e PGNs de interesse.

- **Sintaxe:** `show_model <model_name>`
- **Exemplo:** `show_model DieselBus_ModelA`

### `save_db`
Guarda o estado atual da base de dados de veículos para o ficheiro `vehicles.json` (útil para futuras funcionalidades de edição).

- **Sintaxe:** `save_db`

---

## Outros Comandos

### `help`
Mostra uma lista compacta de todos os comandos disponíveis.

- **Sintaxe:** `help`