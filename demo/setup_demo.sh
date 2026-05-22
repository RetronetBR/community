#!/bin/sh
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
RUNTIME="$ROOT_DIR/runtime"

rm -rf "$RUNTIME"
mkdir -p \
  "$RUNTIME/go1/config" \
  "$RUNTIME/go1/spool/users" \
  "$RUNTIME/go1/spool/peers" \
  "$RUNTIME/go1/spool/transit" \
  "$RUNTIME/go1/spool/deadletter" \
  "$RUNTIME/go1/spool/registry" \
  "$RUNTIME/go1/spool/quarantine" \
  "$RUNTIME/df1/config" \
  "$RUNTIME/df1/spool/users" \
  "$RUNTIME/df1/spool/peers" \
  "$RUNTIME/df1/spool/transit" \
  "$RUNTIME/df1/spool/deadletter" \
  "$RUNTIME/df1/spool/registry" \
  "$RUNTIME/df1/spool/quarantine" \
  "$RUNTIME/clients/marionunes/config" \
  "$RUNTIME/clients/marionunes/drafts" \
  "$RUNTIME/clients/marionunes/inbox" \
  "$RUNTIME/clients/marionunes/outbox" \
  "$RUNTIME/clients/joao/config" \
  "$RUNTIME/clients/joao/drafts" \
  "$RUNTIME/clients/joao/inbox" \
  "$RUNTIME/clients/joao/outbox"

cat > "$RUNTIME/go1/config/node.conf" <<'EOF'
NODE_ID=NOD-000001
NODE_ALIAS=go1@node
HOST=127.0.0.1
PORT=2323
CAPS=MSG,NOTIF,PEER,ROUTE,ACK,STORE,TRANSIT
EOF

cat > "$RUNTIME/df1/config/node.conf" <<'EOF'
NODE_ID=NOD-000002
NODE_ALIAS=df1@node
HOST=127.0.0.1
PORT=2324
CAPS=MSG,NOTIF,PEER,ROUTE,ACK,STORE,TRANSIT
EOF

cat > "$RUNTIME/go1/config/aliases.db" <<'EOF'
marionunes@retronet|USR-000001|NOD-000001
joao@retronet|USR-000002|NOD-000002
EOF

cp "$RUNTIME/go1/config/aliases.db" "$RUNTIME/df1/config/aliases.db"

cat > "$RUNTIME/go1/config/users.db" <<'EOF'
USR-000001|marionunes@retronet|NOD-000001|active|2026-05-22T10:00:00|2026-05-22T10:00:00
USR-000002|joao@retronet|NOD-000002|active|2026-05-22T10:00:00|2026-05-22T10:00:00
EOF

cp "$RUNTIME/go1/config/users.db" "$RUNTIME/df1/config/users.db"

cat > "$RUNTIME/go1/config/trust.db" <<'EOF'
NOD-000001|go1@node|trusted|20
NOD-000002|df1@node|trusted|20
EOF

cp "$RUNTIME/go1/config/trust.db" "$RUNTIME/df1/config/trust.db"

: > "$RUNTIME/go1/config/registry_events.log"
: > "$RUNTIME/df1/config/registry_events.log"

cat > "$RUNTIME/go1/config/peers.db" <<'EOF'
NOD-000002|df1@node|tcp|127.0.0.1|2324|active
EOF

cat > "$RUNTIME/df1/config/peers.db" <<'EOF'
NOD-000001|go1@node|tcp|127.0.0.1|2323|active
EOF

cat > "$RUNTIME/clients/marionunes/config/client.conf" <<'EOF'
USER_ID=USR-000001
ADDRESS=marionunes@retronet
NODE_ID=NOD-000001
NODE_ROOT=../../go1
DEFAULT_NODE=go1@node
DEFAULT_HOST=127.0.0.1
DEFAULT_PORT=2323
DRAFTS_DIR=drafts
AUTO_SYNC=0
AUTO_PUSH=0
AUTO_PULL=0
TOKEN=
EOF

cat > "$RUNTIME/clients/marionunes/drafts/welcome.txt" <<'EOF'
to: joao@retronet
---
Teste de envio RetroNet.
EOF

cat > "$RUNTIME/clients/joao/config/client.conf" <<'EOF'
USER_ID=USR-000002
ADDRESS=joao@retronet
NODE_ID=NOD-000002
NODE_ROOT=../../df1
DEFAULT_NODE=df1@node
DEFAULT_HOST=127.0.0.1
DEFAULT_PORT=2324
DRAFTS_DIR=drafts
AUTO_SYNC=0
AUTO_PUSH=0
AUTO_PULL=0
TOKEN=
EOF

cat > "$RUNTIME/clients/joao/drafts/empty.txt" <<'EOF'
to: marionunes@retronet
---
Resposta de teste.
EOF

echo "Demo runtime created at $RUNTIME"
