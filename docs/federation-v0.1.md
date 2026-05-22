# Federação v0.1

RetroNet não usa controlador central obrigatório.
Cada nó mantém sua própria base local e replica eventos de registry com seus peers.

## Conceitos

- `Alias`: identidade pública do usuário, por exemplo `marionunes@retronet`
- `UserID`: identificador técnico, por exemplo `USR-NOD000001-000001`
- `HomeNode`: nó de origem do usuário, por exemplo `NOD-000001`

## Arquivos

- `users.db`: fonte de verdade dos usuários
- `aliases.db`: índice derivado de roteamento
- `registry_events.log`: trilha append-only dos eventos de identidade
- `trust.db`: estado de confiança do nó e limites operacionais

Formato de `users.db`:

```text
USER_ID|ALIAS|HOME_NODE|STATE|CREATED_AT|UPDATED_AT
```

Exemplo:

```text
USR-NOD000001-000001|marionunes@retronet|NOD-000001|active|2026-05-22T10:00:00|2026-05-22T10:00:00
```

## Estados de usuário

- `pending`
- `active`
- `rejected`
- `conflict`
- `revoked`
- `quarantine`

## Eventos

- `USER_REGISTER`
- `USER_ACCEPT`
- `USER_REJECT`
- `USER_CONFLICT`
- `USER_REVOKE`
- `USER_QUARANTINE`

## Regra de unicidade

O alias `@retronet` deve ser único na rede.

Se um alias já existir como `active`, um novo registro deve ser tratado como `conflict` ou `rejected`, conforme o estado operacional do nó e o processamento do evento.

## Relação com outros documentos

- O fluxo de mensagem usa o protocolo descrito em [protocol-v0.1.md](protocol-v0.1.md)
- O registro manual de usuário aparece em [architecture.md](architecture.md) e no README principal
