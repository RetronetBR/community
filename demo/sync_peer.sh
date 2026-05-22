#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
"$ROOT_DIR/rnpeer" push "$ROOT_DIR/runtime/go1" "$ROOT_DIR/runtime/df1"
echo "peer sync completed"
