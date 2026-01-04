# Changelog

### v4.3.0 (2026-01-04)
- **Recuperação de Funcionalidades e Melhorias:**
  - **GUI Tool (`gui_tool.py`):**
    - A ferramenta gráfica foi completamente refeita com uma interface de abas (Terminal, Gestor de Ficheiros, Configuração).
    - O gestor de ficheiros foi integrado e tornado mais robusto, com suporte para upload/download e uma barra de progresso.
    - O separador de configuração agora apresenta um formulário estruturado para uma edição mais fácil e intuitiva das configurações do dispositivo (WiFi, MQTT, OTA, etc.).
  - **Documentação da API:**
    - Todos os módulos do firmware (`can`, `cli`, `core`, `gps`, `j1939`, `license`, `ota`, `hardware`, `settings`, `time`, `ui`, `wifi`) foram extensivamente documentados no estilo Doxygen.
    - O ficheiro `gemini.md` foi consolidado e atualizado para incluir a documentação de todos os módulos, servindo como uma referência central.
  - **Novos Módulos:**
    - Os módulos `ota` e `hardware` foram restaurados, com a sua estrutura de ficheiros e código base.
  - **Melhorias na UI:**
    - Todos os ecrãs da UI foram refatorados para utilizar uma API centralizada da barra de títulos (`TitleBar`), eliminando a duplicação de código e tornando a gestão de títulos mais consistente.
  - **Melhoria do Firmware Principal:**
    - O ficheiro `j1939_diagnostic_tool.ino` foi refatorado para uma melhor organização, com a inicialização de serviços agrupada em funções separadas.
    - A inclusão de comandos da CLI foi centralizada num único ficheiro de cabeçalho.
  - **Sistema de Configurações:**
    - O sistema de configurações foi melhorado para suportar chaves no formato `categoria.chave` (ex: `wifi.ssid`), tornando a configuração mais estruturada.
    - Adicionada a configuração para o servidor NTP.