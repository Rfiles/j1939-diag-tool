not #!/bin/bash

# Script para inicializar um repositório Git e enviar o projeto para o GitHub.

# --- INSTRUÇÕES ---
# 1. Certifique-se de que tem o Git instalado e configurado (user.name, user.email).
# 2. Crie um novo repositório vazio na sua conta GitHub.
# 3. Edite este ficheiro e substitua "URL_DO_SEU_REPOSITORIO_AQUI" pelo URL do seu repositório.
#    (Ex: https://github.com/seu-usuario/seu-repo.git)
# 4. Dê permissão de execução ao script com o comando: chmod +x deploy_to_github.sh
# 5. Execute o script com: ./deploy_to_github.sh

# --- CONFIGURAÇÃO ---
GITHUB_REPO_URL="https://github.com/Rfiles/j1939-diag-tool.git"
COMMIT_MESSAGE="v1.0.0: Estrutura inicial do projeto e implementação J1939"

# --- SCRIPT ---

# Verifica se o URL do repositório foi alterado
if [ "$GITHUB_REPO_URL" == "URL_DO_SEU_REPOSITORIO_AQUI" ]; then
    echo "ERRO: Por favor, edite o ficheiro 'deploy_to_github.sh' e substitua 'URL_DO_SEU_REPOSITORIO_AQUI' pelo URL do seu repositório GitHub."
    exit 1
fi

# Verifica se o diretório .git já existe
if [ -d ".git" ]; then
    echo "Repositório Git já inicializado."
else
    echo "Inicializando repositório Git..."
    git init -b main
fi

echo "Adicionando ficheiros ao stage..."
git add .

echo "Criando commit..."
git commit -m "$COMMIT_MESSAGE"

# Verifica se o remote 'origin' já existe
if git remote get-url origin > /dev/null 2>&1; then
    echo "Remote 'origin' já existe. A atualizar o URL..."
    git remote set-url origin $GITHUB_REPO_URL
else
    echo "Adicionando remote 'origin'..."
    git remote add origin $GITHUB_REPO_URL
fi

echo "Enviando para o GitHub (branch main)..."
git push -u origin main

echo "\nConcluído! O seu projeto foi enviado para o GitHub."
