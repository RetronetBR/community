# Tipos de nó

Tipos de nó definidos para a rede:

- `EDGE`
- `DIRECT`
- `HUB`
- `BACKBONE`

## Descrição

- `EDGE`: nó de borda, normalmente conectado a usuários finais
- `DIRECT`: nó com rota direta para outros participantes conhecidos
- `HUB`: nó de concentração e redistribuição de tráfego
- `BACKBONE`: nó de infraestrutura com papel de transporte entre regiões

## Relay

O relay é o mecanismo de repasse de mensagens e eventos entre nós.
Ele permite que a rede continue operando sem ponto único de falha.

## Topologia

Exemplo de cadeia de hubs:

```text
hub1 <-> hub2 <-> hub3
```

## Relação com outros documentos

- O comportamento de transporte está em [protocol-v0.1.md](protocol-v0.1.md)
- A visão de rede está em [philosophy.md](philosophy.md)
