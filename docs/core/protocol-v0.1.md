# Protocolo v0.1

Esta é a documentação do protocolo atual da PoC.

## Mensagem do usuário

Formato mínimo:

```text
to: usuario@retronet
---
mensagem
```

O rascunho é criado no cliente, convertido em mensagem final por `rnmsg` e enviado via `rnsync`.

## Fluxo

```text
draft -> outbox -> push -> routing -> peer -> pull -> inbox
```

## Estados no transporte

- `draft`: rascunho ainda editável
- `outbox`: pronto para envio local
- `transit`: em trânsito entre nós
- `inbox`: entregue ao destino
- `deadletter`: destino desconhecido ou inválido
- `quarantine`: conteúdo suspeito ou retido

## Spool

O spool é a área de armazenamento em arquivos texto usada pela rede para transportar conteúdo sem depender de banco de dados.

Diretórios relevantes:

- `spool/users/`
- `spool/peers/`
- `spool/transit/`
- `spool/registry/`
- `spool/deadletter/`
- `spool/quarantine/`

## Deadletter

Mensagens para destinos desconhecidos, inativos ou inválidos devem ir para `deadletter/`.
Isso evita perda silenciosa e mantém rastreabilidade operacional.

## Relação com outros documentos

- O formato de mensagem está em [message-format.md](message-format.md)
- A federação de usuários está em [federation-v0.1.md](federation-v0.1.md)
- A arquitetura está em [architecture.md](architecture.md)
