# Runtime layout

O runtime é baseado em arquivos e diretórios simples.

## Estrutura principal

```text
runtime/
  go1/
    config/
    spool/
  df1/
    config/
    spool/
  clients/
    marionunes/
      config/
      drafts/
      inbox/
      outbox/
    joao/
      config/
      drafts/
      inbox/
      outbox/
```

## Diretórios

- `config/`: configuração do nó ou cliente
- `spool/`: armazenamento operacional da rede
- `spool/users/`: mensagens entregues para usuários locais
- `spool/peers/`: filas para outros nós
- `spool/registry/`: eventos de registro e federação
- `spool/deadletter/`: mensagens que não puderam ser roteadas
- `spool/quarantine/`: conteúdo retido

## Relação com o código

O layout é usado pelos executáveis descritos em [architecture.md](architecture.md) e pelo protocolo descrito em [protocol-v0.1.md](protocol-v0.1.md).
