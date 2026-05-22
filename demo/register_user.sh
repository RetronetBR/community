#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
ALIAS="alice@retronet"

echo "request:"
"$ROOT_DIR/rndctl" user request "$ALIAS" --node-root "$ROOT_DIR/runtime/go1" || true
echo "approve:"
"$ROOT_DIR/rndctl" user approve "$ALIAS" --node-root "$ROOT_DIR/runtime/go1"
echo "rebuild aliases:"
"$ROOT_DIR/rndctl" aliases rebuild --node-root "$ROOT_DIR/runtime/go1"
echo "replicate registry to df1:"
"$ROOT_DIR/rnpeer" push "$ROOT_DIR/runtime/go1" "$ROOT_DIR/runtime/df1"
echo "df1 status:"
"$ROOT_DIR/rndctl" user status "$ALIAS" --node-root "$ROOT_DIR/runtime/df1"
