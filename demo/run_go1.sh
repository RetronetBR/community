#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
LOG_DIR="$ROOT_DIR/runtime/logs"
mkdir -p "$LOG_DIR"
setsid -f "$ROOT_DIR/rnd" "$ROOT_DIR/runtime/go1" > "$LOG_DIR/go1.log" 2>&1
sleep 1
pgrep -f "$ROOT_DIR/rnd $ROOT_DIR/runtime/go1" | head -n 1 > "$LOG_DIR/go1.pid"
echo "go1 started"
