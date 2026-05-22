#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/rnsync" pull "$ROOT_DIR/runtime/clients/joao" "$ROOT_DIR/runtime/df1"
"$ROOT_DIR/rnmsg" read "$ROOT_DIR/runtime/clients/joao/inbox"
