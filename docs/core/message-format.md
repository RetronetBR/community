# Formato de mensagem

RetroNet trabalha com mensagens texto simples.

## Rascunho mínimo

```text
to: joao@retronet
---
Olá
```

## Cabeçalhos internos

Ao enviar uma mensagem, `rnmsg` completa automaticamente os cabeçalhos operacionais usados pela rede, com base no `client.conf` e no estado do sistema.

Os campos operacionais documentados no projeto incluem:

- `From`
- `MsgID`
- `Date`
- `SourceNode`
- `DestUser`
- `DestNode`
- `Status`

## Mensagens de sistema

O mesmo modelo de texto é usado para eventos de registry e mensagens de sistema transportadas entre nós.

Eventos suportados na PoC:

- `USER_REGISTER`
- `USER_ACCEPT`
- `USER_REJECT`
- `USER_CONFLICT`
- `USER_REVOKE`
- `USER_QUARANTINE`

## Relação com outros documentos

- O fluxo de transporte está em [protocol-v0.1.md](protocol-v0.1.md)
- A federação de usuários está em [federation-v0.1.md](federation-v0.1.md)
