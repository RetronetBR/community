# RetroNet POC

PoC em C puro, sem dependências externas, para uma rede store-and-forward inspirada em UUCP/QWK/FidoNet.

Documentação inicial do projeto:

- [docs/README.md](docs/README.md)
- [docs/core/README.md](docs/core/README.md)
- [docs/sdk/README.md](docs/sdk/README.md)
- [docs/research/README.md](docs/research/README.md)

## Componentes

- `rnd`: servidor TCP do nó
- `rnsync`: sincroniza cliente com servidor local
- `rnpeer`: sincroniza mensagens entre nós
- `rnmsg`: cria rascunhos, envia, responde e lê mensagens
- `rndctl`: comandos administrativos simples

## Draft format

O formato mínimo para rascunho é:

```text
to: joao@retronet
---
Teste
```

`rnmsg send` lê esse arquivo, completa os cabeçalhos e grava a mensagem final no `outbox`.
`rnmsg reply` lê uma mensagem recebida, cria um novo rascunho e envia a resposta.

## Registry

- `users.db` é a fonte de verdade para usuários
- `aliases.db` é um índice derivado, reconstruível a partir de `users.db`
- `trust.db` controla o nível de confiança de cada nó
- `registry_events.log` registra as alterações de identidade
- `spool/registry/` transporta eventos `USER_*` entre peers
- `spool/deadletter/` recebe mensagens para aliases desconhecidos ou inativos
- `spool/quarantine/` recebe payloads bloqueados ou suspeitos

Estados de usuário:

- `pending`
- `active`
- `rejected`
- `conflict`
- `revoked`
- `quarantine`

Comandos principais:

- `rndctl user request <alias> --node-root <node_root>`
- `rndctl user approve <alias> --node-root <node_root>`
- `rndctl user reject <alias> --node-root <node_root>`
- `rndctl user conflict <alias> --node-root <node_root>`
- `rndctl user status <alias> --node-root <node_root>`
- `rndctl user list --node-root <node_root>`
- `rndctl aliases rebuild --node-root <node_root>`

## Client Flow

- `rnmsg draft <client_root> <to> <body...>` cria um rascunho no diretório `drafts/`
- `rnmsg send <client_root> <draft_file>` completa os cabeçalhos e envia para o nó local
- `rnmsg --sync <client_root>` faz push e pull usando o nó configurado no `client.conf`
- `rnmsg reply <client_root> <inbox_message> <body...>` responde offline e envia

## Build

```sh
make
```

## Demo

1. `make`
2. `./demo/setup_demo.sh`
3. `./demo/run_go1.sh`
4. `./demo/run_df1.sh`
5. `./demo/send_test.sh`
6. `./demo/sync_peer.sh`
7. `./demo/read_joao.sh`

Registry demo opcional:

1. `./demo/register_user.sh`
2. `./demo/duplicate_user_test.sh`

## Manual Tutorial

Para subir um nó manualmente e registrar um usuário:

1. Compile o projeto:

```sh
make
```

2. Prepare o runtime:

```sh
./demo/setup_demo.sh
```

3. Abra um terminal para o primeiro nó e suba o servidor. Deixe este processo em foreground:

```sh
./rnd runtime/go1
```

4. Abra um segundo terminal para comandos administrativos. Em outro terminal, se quiser testar a federação, suba também o segundo nó:

```sh
./rnd runtime/df1
```

5. Registre um usuário novo no nó `go1` a partir do terminal administrativo:

```sh
./rndctl user request bob@retronet --node-root runtime/go1
```

6. Aprove o usuário para torná-lo `active`:

```sh
./rndctl user approve bob@retronet --node-root runtime/go1
```

7. Reconstrua o índice de aliases a partir de `users.db`:

```sh
./rndctl aliases rebuild --node-root runtime/go1
```

8. Consulte o status local do usuário:

```sh
./rndctl user status bob@retronet --node-root runtime/go1
```

9. Se quiser transportar a identidade para outro nó, replique os eventos de registry:

```sh
./rnpeer push runtime/go1 runtime/df1
```

10. Verifique no outro nó:

```sh
./rndctl user status bob@retronet --node-root runtime/df1
```

11. Liste os usuários para conferir o estado final:

```sh
./rndctl user list --node-root runtime/go1
```

12. Depois disso, o usuário já pode criar mensagem com o formato mínimo:

```text
to: joao@retronet
---
Teste
```

13. Use `rnmsg draft`, `rnmsg send` e `rnmsg --sync` para enviar e sincronizar a mensagem.

Para encerrar o nó manualmente, use `Ctrl+C` no terminal onde `rnd` está rodando.

## Layout

```text
retronet-poc/
  Makefile
  README.md
  docs/
  src/
  demo/
  runtime/
```

## Protocol

- `HELLO <client-or-node-id>`
- `PUSH`
- `SIZE <bytes>`
- `<conteúdo>`
- `END`
- `PULL <user-id>`
- `ACK <msgid>`
- `BYE`

## Observações

- Não há TLS.
- Não há autenticação real.
- Os dados são armazenados apenas em arquivos texto.
- A PoC resolve alias por `aliases.db` e encaminha para o `node_id` correto usando `peers.db`.
