#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
ALIAS="alice@retronet"

if "$ROOT_DIR/rndctl" user request "$ALIAS" --node-root "$ROOT_DIR/runtime/df1"; then
  echo "unexpected: duplicate alias accepted"
  exit 1
else
  echo "duplicate alias rejected on df1"
fi
"$ROOT_DIR/rndctl" user status "$ALIAS" --node-root "$ROOT_DIR/runtime/df1" || true
