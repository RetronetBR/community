#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
DRAFT=$("$ROOT_DIR/rnmsg" draft "$ROOT_DIR/runtime/clients/marionunes" "joao@retronet" "Teste de envio RetroNet.")
"$ROOT_DIR/rnmsg" send "$ROOT_DIR/runtime/clients/marionunes" "$DRAFT"
echo "message queued in outbox"
