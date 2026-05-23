# Notificações TSR no RetroNet

## Status

Pesquisa / arquitetura inicial.

Não há implementação funcional documentada neste arquivo.

## Objetivo

Documentar o popup temporário do RNTSR.

O popup deve avisar sobre eventos pendentes sem transformar o RNTSR em
cliente completo.

## Comportamento

- aparece quando o polling detecta evento
- some após poucos segundos
- não interrompe o usuário por muito tempo
- salva a região da tela antes de desenhar
- restaura a região após desaparecer
- só deve aparecer em modo texto
- não deve aparecer em modo gráfico
- não deve aparecer sobre jogos
- não deve aparecer sobre aplicações fullscreen sensíveis
- beep pode ser fallback
- estado interno pode ser fallback
- futuramente pode integrar com `librnaudio`

## Exemplo visual

Exemplo conceitual:

```text
┌──────────────────────────────┐
│ RetroNet Community           │
│ Nova mensagem disponível     │
└──────────────────────────────┘
```

## Regras de segurança

- não redesenhar tela se o modo de vídeo não for texto
- evitar escrita direta em vídeo quando a aplicação estiver em modo gráfico
- restaurar exatamente a região usada pelo popup
- não bloquear o sistema esperando rede
- não fazer polling durante desenho do popup
- não tocar alerta sonoro longo dentro de rotina crítica
- não exibir popup sobre jogos
- não exibir popup sobre aplicações fullscreen sensíveis

## Estado mínimo da notificação

Campos conceituais:

- `type`
- `priority`
- `ttl_seconds`
- `flags`
- `counter`
- `last_event_id`

## Tipos de notificação

- `MAIL_PENDING`
- `BOARD_UPDATED`
- `CHAT_PENDING`
- `MOTD_CHANGED`
- `FILE_AVAILABLE`
- `SYSTEM_EVENT`
- `SESSION_WARNING`

## Configuração futura

Exemplo conceitual:

```ini
[agent]
popup_enabled=yes
popup_seconds=4
popup_position=top_right
beep=yes
```

## Cuidados

- não exibir popup em modo gráfico
- não exibir sobre jogos
- não exibir sobre aplicações fullscreen sensíveis
- usar beep como fallback
- usar estado interno como fallback
- não bloquear esperando rede
- não transformar popup em cliente de mensagens

## Alertas sonoros futuros

- PC Speaker
- AdLib/OPL
- Sound Blaster 16 futuramente
- integração futura com `librnaudio`

## Relação com RNTSR

O RNTSR deve apenas avisar que existe algo pendente.

O conteúdo completo deve ser obtido por clientes apropriados, como
RNMAIL, RNBOARD, RNCHAT ou RNMSG.

## Próximos passos

- definir política de popup por tipo de evento
- documentar integração com áudio
- mapear restauração de tela em ambientes sensíveis
- definir fallback para modo gráfico
- definir fallback para quiet mode

## Relação com outros documentos

- [RNTSR](tsr.md)
- [Packet Driver](packet-driver.md)
- [Áudio](audio.md)
