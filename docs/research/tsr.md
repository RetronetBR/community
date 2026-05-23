# TSR

## Objetivo

Documentar a trilha de pesquisa para `RNTSR`.

## Status

O TSR é uma ideia de cliente residente para polling leve, alertas e popups curtos.

## Regras desejadas

- carregar somente se o Packet Driver estiver carregado
- ser pequeno
- fazer polling leve
- não baixar mensagens completas
- apenas consultar o servidor
- exibir popup curto quando houver nova mensagem, aviso ou evento
- desaparecer após alguns segundos
- poder tocar alerta sonoro futuramente

## Uso futuro

- notificação de mensagens
- alertas de board
- eventos de sistema

## Próximos passos

- mapear integrações com `INT 2Fh`
- documentar estratégia de polling
- definir limites de memória e de execução
