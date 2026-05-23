# Protocolo

## Objetivo

Definir o contrato de comunicação do SDK com a rede RetroNet sem quebrar clientes antigos.

## Princípios

- protocolo fixo, mas extensível
- clientes antigos não devem quebrar
- o servidor entrega apenas recursos compatíveis com o cliente
- cada programa pode habilitar capabilities específicas

## Exemplo de fluxo

```text
cliente -> connect -> auth -> poll -> close
```

## Status

O protocolo do SDK deve acomodar as capacidades do ecossistema sem
impor peso desnecessário a clientes simples.

## Uso futuro

- autenticação opcional
- notificação de mensagens
- boards
- QWK
- arquivos
- IPX

## Próximos passos

- formalizar mensagens mínimas
- documentar negociação de capabilities
- separar transporte da semântica de aplicação
