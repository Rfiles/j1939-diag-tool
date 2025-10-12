# Ferramenta de Diagnóstico J1939 para ESP32

**Versão Atual:** 3.4.0

## Sobre o Projeto

Esta é uma plataforma de diagnóstico J1939 de nível profissional para o ESP32, desenhada para ser robusta, modular e extensível. O firmware foi construído de raiz com uma arquitetura multi-tarefa (FreeRTOS), permitindo uma operação concorrente e não-bloqueante, ideal para a análise de redes de veículos em tempo real.

O sistema é capaz de comunicar com redes J1939, interpretar protocolos complexos, e enviar telemetria para plataformas de IoT. Inclui um sistema de licenciamento por hardware e uma base de dados de veículos para permitir diagnósticos específicos por modelo, tornando-o uma base poderosa para uma ferramenta de diagnóstico comercial.

## Arquitetura do Software

O coração do projeto é uma arquitetura modular baseada em tarefas FreeRTOS, onde cada responsabilidade principal é isolada no seu próprio módulo e tarefa.

- **`j1939_handler`**: Gere a camada física do CAN e implementa a lógica de **Address Claiming** para garantir uma integração segura na rede.
- **`tp_handler`**: Implementa o **Protocolo de Transporte (TP)** do J1939, permitindo a remontagem de mensagens multi-pacote (BAM), essencial para ler dados de diagnóstico (DM) e outras informações extensas.
- **`pdu_processor`**: Atua como o cérebro da análise de dados, recebendo mensagens completas do `tp_handler` e descodificando PGNs específicos, como o VIN.
- **`comms_handler`**: Gere toda a conectividade de rede (Wi-Fi) e a comunicação com a plataforma de IoT (MQTT), incluindo a sincronização de tempo via NTP.
- **`cli_handler`**: Oferece uma interface de linha de comandos modular e extensível para interagir com todas as funcionalidades do sistema.
- **`license_handler`**: Gere um sistema de licenciamento avançado, com chaves ligadas ao hardware, usos limitados, e funcionalidades ativáveis.
- **`vehicle_db_handler`**: Carrega e gere uma base de dados de modelos de veículos a partir de um ficheiro `vehicles.json`, permitindo um diagnóstico específico por modelo.

## Funcionalidades Implementadas

- **Stack J1939:**
  - **Address Claiming:** Negociação de endereço na rede para evitar conflitos.
  - **Transport Protocol (TP):** Suporte para receber mensagens multi-pacote (BAM).
- **Diagnóstico:**
  - **Leitura de VIN:** Comando `request_vin` para pedir e descodificar o Número de Identificação do Veículo (PGN 65260).
  - Base para leitura de Mensagens de Diagnóstico (DM) através do TP.
  - Implementação de **DM11** e **DM13**.
- **Conectividade e Telemetria:**
  - Ligação a redes **Wi-Fi**.
  - Publicação de dados e eventos para um broker **MQTT**.
  - Sincronização da hora do sistema com um servidor **NTP** para timestamps precisos.
- **Sistema de Licenciamento:**
  - Geração de chaves de 12 caracteres ligadas ao ID de hardware único do dispositivo.
  - Chaves com número de usos, flags de funcionalidades e data de validade para ativação.
  - Proteção contra brute-force e uso único de chaves.
- **Base de Dados de Veículos:**
  - Carregamento de uma base de dados de modelos de autocarros (Diesel e Elétricos) a partir de um ficheiro `vehicles.json`.
  - Comandos na CLI para listar e visualizar os modelos da base de dados.
- **Configuração Flexível:**
  - Configurações por defeito carregadas de `config.json`.
  - Configurações do utilizador (incluindo senhas) guardadas de forma segura na **NVS**.
  - Gestão completa das configurações via CLI, sem necessidade de recompilar.
- **Atualizações OTA:** Capacidade de atualizar o firmware remotamente via Wi-Fi.

## Como Começar

1.  **Hardware:** Precisarás de uma placa **LilyGo T-Display S3**, um transceiver CAN **MCP2515** e, opcionalmente, um módulo GPS **u-blox NEO-M8N**.
2.  **Compilação:** É recomendado usar **PlatformIO**. Clona o projeto e executa `pio run` para compilar e `pio run --target upload` para carregar.
3.  **Configuração Inicial:** Após o primeiro arranque, liga-te via monitor série (115200 baud) e configura a tua rede Wi-Fi:
    ```bash
    set wifi_ssid "O_NOME_DA_TUA_REDE"
    set wifi_pass "A_TUA_SENHA"
    save
    ```
4.  **Ativação da Licença:**
    - Obtém o ID do teu dispositivo com o comando: `get_hw_id`
    - Usa o script `generate_license.py` para criar uma chave de licença.
    - Ativa a licença no dispositivo com: `activate <a_tua_chave_de_licenca>`

## Utilização

Após a configuração, podes começar a interagir com o dispositivo. Para uma lista completa e detalhada de todos os comandos, consulta o ficheiro:

**[➡️ Documentação Completa da CLI (CLI_COMMANDS.md)](CLI_COMMANDS.md)**

## Roadmap de Futuras Funcionalidades

Este projeto tem uma base sólida que permite a implementação de muitas outras funcionalidades profissionais:

- **Interpretação de Mensagens de Diagnóstico (DM):**
  - Implementar a descodificação completa de mensagens **DM1** (Erros Ativos) e **DM2** (Erros Passivos) para extrair os códigos de erro (SPN, FMI, OC).
  - Adicionar comandos para limpar os erros (**DM3**).
- **Leitura de Dados por Centralina:**
  - Implementar o comando `read_ecu <nome_ecu>` para pedir todos os PGNs de interesse de uma centralina específica, com base no modelo de veículo selecionado.
- **Gestão da Base de Dados via CLI:**
  - Adicionar comandos para criar e editar modelos de veículos e ECUs diretamente a partir da linha de comandos (`add_model`, `add_ecu`, etc.).
- **Transferência de Ficheiros:**
  - Implementar comandos `upload_file` e `download_file` para gerir remotamente os ficheiros no sistema LittleFS (ex: `vehicles.json`).
- **Melhorias na Interface de Utilizador (UI):**
  - Mostrar no ecrã do dispositivo os dados lidos em tempo real, os erros de diagnóstico, e o estado da licença.

---

Para o histórico de alterações completo, consulta o ficheiro `changelog.md`.