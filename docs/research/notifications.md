# Notificações TSR no RetroNet

## Objetivo

Documentar o popup temporário do RNTSR.

## Status

Pesquisa / arquitetura inicial.

## Comportamento

- aparece quando o polling detecta evento
- some após poucos segundos
- não interrompe o usuário por muito tempo
- restaura a área da tela após desaparecer
- o popup é temporário
- o popup salva e restaura a área da tela
- o popup só deve aparecer em modo texto
- em modo gráfico, fullscreen, jogo ou aplicação sensível, não exibir popup
- beep pode ser fallback
- futuramente pode integrar com `librnaudio`

## Exemplo visual

    ┌──────────────────────────────┐
    │ RetroNet Community           │
    │ Nova mensagem disponível     │
    └──────────────────────────────┘

## Regras de segurança

- não redesenhar tela se modo de vídeo não for texto
- evitar escrita direta em vídeo quando aplicação estiver em modo gráfico
- restaurar exatamente a região usada pelo popup
- não bloquear o sistema esperando rede
- não fazer polling durante desenho do popup
- não tocar alerta sonoro longo dentro de rotina crítica

## Estado mínimo da notificação

Campos conceituais:

- `type`
- `priority`
- `ttl_seconds`
- `flags`
- `counter`
- `last_event_id`

## Configuração futura

    [agent]
    popup_enabled=yes
    popup_seconds=4
    popup_position=top_right
    beep=yes

## Alertas sonoros futuros

- PC Speaker
- AdLib/OPL
- Sound Blaster 16 futuramente

## Tipos de notificação

- `MAIL_PENDING`
- `BOARD_UPDATED`
- `CHAT_PENDING`
- `MOTD_CHANGED`
- `FILE_AVAILABLE`
- `SYSTEM_EVENT`
- `SESSION_WARNING`

## Próximos passos

- definir política de popup por tipo de evento
- documentar integração com áudio
- mapear restauração de tela em ambientes sensíveis

## Relação com outros documentos

- [RNTSR](tsr.md)
- [Packet Driver](packet-driver.md)
- [Áudio](audio.md)
