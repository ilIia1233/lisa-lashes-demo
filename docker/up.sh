#!/bin/bash
# Wrapper around docker compose that injects secrets/.env for variable
# substitution (DB_NAME, DB_USER, DB_PASSWORD, PORT → used in compose.yaml).
#
# Usage (from any directory):
#   ./docker/up.sh up -d        # start
#   ./docker/up.sh down         # stop
#   ./docker/up.sh logs -f      # follow logs
#   ./docker/up.sh build        # rebuild images

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ENV_FILE="$SCRIPT_DIR/../secrets/.env"
COMPOSE_FILE="$SCRIPT_DIR/compose.yaml"

if [[ ! -f "$ENV_FILE" ]]; then
  echo "ERROR: $ENV_FILE not found."
  echo "Copy secrets/.env.example to secrets/.env and fill in your credentials."
  exit 1
fi

if docker compose version &>/dev/null; then
  exec docker compose \
    --env-file "$ENV_FILE" \
    -f "$COMPOSE_FILE" \
    "$@"
else
  exec docker-compose \
    --env-file "$ENV_FILE" \
    -f "$COMPOSE_FILE" \
    "$@"
fi
