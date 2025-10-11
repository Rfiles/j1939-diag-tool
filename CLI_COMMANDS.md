# Documentação da Interface de Linha de Comandos (CLI)

Esta é a documentação para a Interface de Linha de Comandos (CLI) da Ferramenta de Diagnóstico J1939.

## Comandos Disponíveis

### `help`

Mostra a mensagem de ajuda com a lista de todos os comandos disponíveis.

**Sintaxe:**
```
help
```

---

### `request <pgn>`

Envia um pedido de PGN (Parameter Group Number) para a rede J1939.

**Sintaxe:**
```
request <pgn>
```

**Parâmetros:**
- `<pgn>`: O número do PGN a ser solicitado (em formato decimal).

**Exemplo:**
```
request 65262
```

---

### `set <key> <value>`

Define o valor de uma chave de configuração na memória do dispositivo. Esta alteração não é persistente e será perdida ao reiniciar, a menos que o comando `save` seja executado.

**Sintaxe:**
```
set <key> <value>
```

**Parâmetros:**
- `<key>`: A chave de configuração a ser alterada.
- `<value>`: O novo valor para a chave. Se o valor contiver espaços, deve ser colocado entre aspas.

**Chaves Disponíveis:**
- `wifi_ssid`: Nome da rede Wi-Fi (SSID).
- `wifi_pass`: Senha da rede Wi-Fi.
- `mqtt_broker`: Endereço do servidor MQTT.
- `mqtt_port`: Porta do servidor MQTT (ex: 1883).
- `mqtt_topic`: Tópico MQTT para publicação de dados.
- `ota_host`: Nome do dispositivo (hostname) para as atualizações OTA.
- `ota_pass`: Senha para as atualizações OTA.

**Exemplo:**
```
set wifi_ssid "A Minha Rede Wi-Fi"
set wifi_pass AMinhaSenhaSuperSegura123
set mqtt_broker 192.168.1.100
```

---

### `save`

Guarda todas as configurações atuais (que estão em memória) para a memória persistente (NVS - Non-Volatile Storage). As configurações guardadas serão carregadas automaticamente no próximo arranque do dispositivo.

**Sintaxe:**
```
save
```

**Exemplo de Utilização (Configurar e Guardar Wi-Fi):**
```
set wifi_ssid AMinhaRede
set wifi_pass AMinhaSenha
save
```
