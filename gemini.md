# Visão Geral do Projeto: Ferramenta de Diagnóstico J1939

Este projeto é uma ferramenta de diagnóstico para a rede de comunicação de veículos J1939, implementada num microcontrolador (compatível com Arduino). O objetivo é ler e interpretar mensagens da rede J1939, e exibir informações de diagnóstico num ecrã.

# Estrutura do Projeto

- `j1939_diagnostic_tool.ino`: O ficheiro principal do sketch Arduino.
- `src/`: Contém o código fonte principal, dividido em módulos.
- `src/core/`: Lógica principal da aplicação.
- `src/j1939/`: Lógica para lidar com o protocolo J1939.
- `src/driver/`: Drivers para o hardware (ecrã, botões, etc.).
- `data/`: Ficheiros de configuração.

# Como Compilar e Carregar

Este projeto usa PlatformIO.
- Para compilar: `pio run`
- Para carregar para o dispositivo: `pio run --target upload`

# Convenções de Desenvolvimento

- Usamos o estilo de código do Google para C++.
- Os nomes de variáveis e funções devem ser em `snake_case`.
- Os nomes de classes devem ser em `PascalCase`.

# Comandos Úteis

- `pio test`: Executar os testes.
- `pio check`: Verificar o código com o linter.
