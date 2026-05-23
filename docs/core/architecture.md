# Arquitetura

Arquitetura atual da PoC:

```text
Cliente
  ↓
rnsync
  ↓
rnd
  ↓
rnpeer
  ↓
rnd
  ↓
rnsync
  ↓
Cliente destino
```

## Executáveis

- `rnd`: servidor TCP do nó
- `rnsync`: sincronização entre cliente e nó local
- `rnpeer`: replicação entre nós
- `rnmsg`: criação, leitura, resposta e envio de mensagens
- `rndctl`: comandos administrativos simples

## Responsabilidades

- `rnmsg` trabalha com rascunhos e mensagens no formato mínimo
- `rnsync` faz o transporte entre cliente e nó local
- `rnd` recebe, valida e roteia mensagens
- `rnpeer` replica conteúdo entre nós
- `rndctl` manipula registro de usuários e dados administrativos

## Onde esta arquitetura se encaixa

O desenho segue a filosofia descrita em [philosophy.md](philosophy.md) e o fluxo de troca descrito em [protocol-v0.1.md](protocol-v0.1.md).
