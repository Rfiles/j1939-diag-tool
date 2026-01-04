#!/bin/bash

# Script avançado para enviar alterações para o GitHub.

# --- Visão Geral ---
# Este script automatiza o processo de adicionar, commitar e enviar alterações para o GitHub.
# Ele suporta:
# - Mensagens de commit personalizadas via argumentos.
# - Deteção automática do branch atual.
# - Envio para um branch específico.
# - Criação e envio de tags para versionamento (releases).
# - Um modo 'force-push' com aviso.
# - Um ficheiro de configuração para o URL do repositório.

set -e # Termina o script se qualquer comando falhar

CONFIG_FILE="deploy_config.sh"

# --- Funções ---

# Função para exibir a ajuda
usage() {
    echo "Uso: $0 [opções]"
    echo
    echo "Opções:"
    echo "  -m <mensagem>   Define a mensagem de commit (obrigatório)."
    echo "  -b <branch>     Define o branch para onde enviar (padrão: branch atual)."
    echo "  -t <tag>        Cria uma nova tag para a release (ex: v1.0.0)."
    echo "  --force         Força o push (use com cuidado!)."
    echo "  -h, --help      Mostra esta mensagem de ajuda."
    echo
    exit 1
}

# --- Lógica Principal ---

# 1. Carregar Configuração
# -------------------------
if [ ! -f "$CONFIG_FILE" ]; then
    echo "ERRO: Ficheiro de configuração '$CONFIG_FILE' não encontrado."
    exit 1
fi
source "./$CONFIG_FILE"

if [ "$GITHUB_REPO_URL" == "URL_DO_SEU_REPOSITORIO_AQUI" ]; then
    echo "ERRO: O URL do repositório em '$CONFIG_FILE' não está configurado."
    exit 1
fi

# 2. Analisar Argumentos da Linha de Comando
# ------------------------------------------
COMMIT_MESSAGE=""
TARGET_BRANCH=""
NEW_TAG=""
FORCE_PUSH=false

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -m) COMMIT_MESSAGE="$2"; shift ;;
        -b) TARGET_BRANCH="$2"; shift ;;
        -t) NEW_TAG="$2"; shift ;;
        --force) FORCE_PUSH=true ;;
        -h|--help) usage ;;
        *) echo "Opção desconhecida: $1"; usage ;;
    esac
    shift
done

# Validação da mensagem de commit
if [ -z "$COMMIT_MESSAGE" ]; then
    echo "ERRO: A mensagem de commit é obrigatória. Use a flag -m."
    usage
fi

# 3. Preparar para o Push
# -----------------------
# Verifica se há alterações para commitar
if git diff-index --quiet HEAD --; then
    echo "Não há alterações para commitar. A sair."
    exit 0
fi

# Define o branch de destino (se não for especificado, usa o atual)
if [ -z "$TARGET_BRANCH" ]; then
    TARGET_BRANCH=$(git rev-parse --abbrev-ref HEAD)
    echo "Nenhum branch especificado. Usando o branch atual: '$TARGET_BRANCH'."
fi

echo "Adicionando todos os ficheiros ao stage..."
git add .

echo "Criando commit com a mensagem: '$COMMIT_MESSAGE'..."
git commit -m "$COMMIT_MESSAGE"

# 4. Criar Tag (se especificado)
# ------------------------------
if [ -n "$NEW_TAG" ]; then
    echo "Criando nova tag: $NEW_TAG"
    git tag "$NEW_TAG"
fi

# 5. Enviar para o GitHub
# -----------------------

# Constrói a URL do repositório com o token, se disponível
if [ -n "$GITHUB_TOKEN" ]; then
    # Remove "https://" para evitar duplicação e extrai o host/path
    REPO_URL_NO_PROTOCOL=${GITHUB_REPO_URL#https://}
    REMOTE_URL="https://${GITHUB_TOKEN}@${REPO_URL_NO_PROTOCOL}"
    echo "Usando autenticação via token para o push."
else
    REMOTE_URL=$GITHUB_REPO_URL
    echo "Nenhum token encontrado. Usando a URL padrão do repositório."
fi

# Define a flag de force-push
PUSH_CMD="git push"
if [ "$FORCE_PUSH" = true ]; then
    read -p "AVISO: Tem a certeza que quer forçar o push? Isto pode sobrescrever o histórico no remote. (s/n): " confirm
    if [[ "$confirm" == "s" || "$confirm" == "S" ]]; then
        PUSH_CMD="git push --force"
        echo "Force push ativado."
    else
        echo "Push cancelado."
        exit 1
    fi
fi

# Configura o remote se necessário, usando a URL com o token
if ! git remote get-url origin > /dev/null 2>&1; then
    echo "Adicionando remote 'origin'..."
    git remote add origin "$REMOTE_URL"
else
    # Garante que o remote 'origin' está a usar a URL correta (com ou sem token)
    git remote set-url origin "$REMOTE_URL"
fi

echo "Enviando alterações para o branch '$TARGET_BRANCH'..."
$PUSH_CMD origin "$TARGET_BRANCH"

# Envia a tag se foi criada
if [ -n "$NEW_TAG" ]; then
    echo "Enviando a tag '$NEW_TAG'..."
    git push origin "$NEW_TAG"
fi

# Limpa o remote para remover o token de acesso.
echo "Limpando a URL do remote para remover o token de acesso."
git remote set-url origin "$GITHUB_REPO_URL"

echo -e "\n✅ Concluído! O seu projeto foi enviado com sucesso."