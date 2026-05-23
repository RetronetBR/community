# Capabilities

## Objetivo

Permitir expansão gradual sem quebrar compatibilidade.

## Conceito

O protocolo é estável, mas os recursos disponíveis podem variar conforme cliente, plataforma e transporte.
O servidor deve oferecer apenas o que o cliente declarou suportar.

## Exemplos

- `RN_CAP_AUTH`
- `RN_CAP_MOTD`
- `RN_CAP_CHAT`
- `RN_CAP_BOARD`
- `RN_CAP_QWK`
- `RN_CAP_FILE`
- `RN_CAP_IPX`
- `RN_CAP_NOTIFY`
- `RN_CAP_AUDIO`
- `RN_CAP_VGA`

## Status

Esses nomes são a base conceitual de negociação de recursos no SDK.

## Uso futuro

- perfis por cliente
- fallback para clientes antigos
- expansão sem quebra de contrato

## Próximos passos

- definir bitmap ou texto para troca de capabilities
- mapear capability para módulo do SDK
