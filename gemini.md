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

Utilizar vector e map para programação de funções quando aplicavel.
Ser menos verboso nas respostas. Explica so se solicitado.

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
