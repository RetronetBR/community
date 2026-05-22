# Capabilities

As capabilities já definidas para a rede são:

- `MSG`
- `NOTIF`
- `PEER`
- `ROUTE`
- `ACK`
- `STORE`
- `TRANSIT`

## Reservadas

As seguintes capabilities estão reservadas para expansão futura:

- `FILE`
- `CHAT`
- `BBS`
- `FTP`
- `BUNDLE`
- `SERIAL`

## Objetivo

O conjunto de capabilities existe para permitir evolução sem quebrar a compatibilidade do núcleo.
A ideia é crescer em cima da base store-and-forward, sem adicionar dependências pesadas nem mudar a filosofia da rede.

## Relação com outros documentos

- O transporte base está em [protocol-v0.1.md](protocol-v0.1.md)
- A arquitetura atual está em [architecture.md](architecture.md)
