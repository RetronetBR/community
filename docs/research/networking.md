# Networking

## Objetivo

Registrar a trilha de pesquisa em transportes e redes para o ecossistema RetroNet.

## Status

O SDK deve funcionar bem em redes lentas, transportes legados e integrações simples.
O RNTSR não implementa stack de rede própria.
Ele usa a camada de rede disponível no ambiente.

## Uso futuro

- TCP/IP
- Packet Driver
- PPP
- SLIP
- Serial
- ESP32 como modem Wi-Fi/serial
- IPX
- IPXBox
- gateways legados

Para DOS, TCP/IP via Packet Driver e mTCP é um caminho provável.
Para cenários legados, PPP, SLIP, serial e modem Wi-Fi baseado em ESP32 continuam interessantes.
Para jogos e ambientes DOS, IPX e IPXBox seguem como pesquisa futura.

## Modelo de rede para RNTSR

- handshake mínimo
- heartbeat
- poll de status
- resposta curta
- sem download de conteúdo
- backoff em erro
- timeout curto
- sem travar a máquina do usuário

## Regra

O RNTSR não deve implementar stack de rede própria.
Ele deve usar uma camada disponível no ambiente.

## Diretrizes

- usar a rede disponível sem impor stack própria ao residente
- manter o protocolo leve para redes lentas
- separar transporte da semântica da aplicação

## Próximos passos

- separar transporte moderno de transporte legado
- documentar o mínimo necessário para clientes DOS
