# Networking

## Objetivo

Documentar os transportes e caminhos de rede que o SDK pode abranger.

## Transportes e redes

- TCP/IP
- Packet Driver
- PPP
- SLIP
- Serial
- ESP32 como modem Wi-Fi/serial
- IPX
- IPXBox
- gateways legados

## Uso futuro

- cliente moderno para teste
- cliente DOS com Packet Driver
- ponte para redes antigas
- integração com hardware serial

## Status

O SDK deve tratar o transporte como camada intercambiável sempre que possível.

## Próximos passos

- documentar perfis por transporte
- separar rede moderna de rede legada
- mapear dependências por plataforma
