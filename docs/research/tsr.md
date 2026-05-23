# RNTSR — RetroNet TSR Agent

## Status

Pesquisa / arquitetura inicial.

## Objetivo

- agente residente mínimo para MS-DOS e FreeDOS
- consulta periódica ao servidor RetroNet
- notifica o usuário quando houver eventos
- mantém o cliente leve e modular

## Ciclo de vida

O RNTSR deve ter fases claramente separadas:

- instalação
- execução residente
- remoção segura

Antes de instalar, o agente deve verificar:

- se já existe uma instância carregada
- se há Packet Driver disponível
- se a configuração mínima está válida
- se o ambiente permite rede

Se qualquer pré-condição falhar, a instalação deve abortar sem deixar TSR residente.
Isso evita TSR zumbi e mantém o sistema previsível.

## O que o RNTSR faz

- verifica sessão
- heartbeat
- polling leve
- consulta de eventos
- detecção de novidades
- popup temporário
- atualização de estado local mínimo
- pode expor interface local futura para clientes DOS

## O que o RNTSR não faz

- não baixa mensagens completas
- não baixa boards
- não baixa anexos
- não processa QWK
- não transfere arquivos
- não substitui RNMAIL, RNBOARD, RNCHAT ou RNMSG

## Fases do RNTSR

O RNTSR deve ser pensado em fases separadas:

1. instalação
2. código residente
3. código descartável após instalação
4. possível rotina futura de remoção

O código de instalação não deve permanecer em memória se não for necessário.

## Dependência de Packet Driver

O RNTSR só deve carregar se houver Packet Driver carregado.
Se não detectar Packet Driver, deve abortar sem ficar residente.

O motivo é simples: evitar TSR zumbi sem rede.

## Cuidados de arquitetura

- o TSR não deve chamar rotinas pesadas durante interrupções de timer
- o TSR deve evitar reentrância indevida em DOS
- o TSR deve manter estado mínimo
- o TSR deve usar flags internas para adiar trabalho pesado até um momento seguro
- o TSR deve preservar registradores, flags e contexto da aplicação interrompida
- o TSR deve ser pequeno e previsível
- o TSR deve permitir modo quiet/silent
- o TSR deve permitir desativar popup sem descarregar o agente

## Separação de código

Implementações futuras devem separar claramente:

- código de instalação
- código residente
- código descartável após instalação

Também devem estudar:

- Program Segment Prefix
- cálculo de memória a manter residente
- preservação e restauração de vetores de interrupção
- detecção de instância já instalada
- remoção segura
- troca e preservação de stack
- cuidado com chamadas DOS a partir de interrupções

## Permanência residente

Exemplo conceitual de Assembly:

    ; RNTSR - exemplo conceitual
    ; Mantém residente apenas a parte necessária.
    ; AH=31h: DOS Keep Process
    ; DX: tamanho residente em parágrafos
    ;
    ; Este exemplo não é implementação final.

    mov ah, 31h
    mov dx, resident_size_paragraphs
    int 21h

INT 21h/AH=31h deve ser o caminho preferencial para novos TSRs.
INT 27h é legado e deve ser evitado salvo necessidade específica de compatibilidade.
O tamanho residente precisa ser calculado com cuidado.
O código descartável não deve ficar residente.
O cálculo deve considerar PSP, segmentos e final real do código residente.

## Fluxo de inicialização

```text
AUTOEXEC.BAT
  ↓
Packet Driver
  ↓
RNTSR /LOAD
  ↓
detecção de rede
  ↓
handshake mínimo
  ↓
instalação residente
```

## Comandos previstos

- `RNTSR /LOAD`
- `RNTSR /UNLOAD`
- `RNTSR /STATUS`
- `RNTSR /POLLNOW`
- `RNTSR /QUIET`

## Modelo de polling

- Idle: 120s
- Ativo: 30s
- Erro: backoff 5min, 10min, 30min

## Uso futuro

- notificação de mensagens
- alertas de board
- eventos de sistema

## Eventos previstos

- MAIL
- BOARD
- CHAT
- SYSTEM
- MOTD
- FILE
- USER

## Capabilities relacionadas

- `RN_CAP_NOTIFY`
- `RN_CAP_AUDIO`
- `RN_CAP_MOTD`

## Integração futura via interrupção

Uma possibilidade futura é expor consulta de estado via interrupção dedicada, por exemplo `INT 60h`.

Exemplo conceitual:

    ; Consulta local ao RNTSR
    ; Exemplo conceitual, não implementação final.

    mov ah, 00h
    int 60h
    jc  rn_error

Exemplo conceitual de funções:

- `AH=00h` -> status do agente
- `AH=01h` -> quantidade de eventos pendentes
- `AH=02h` -> forçar poll
- `AH=03h` -> modo quiet on/off
- `AH=04h` -> consultar última notificação
- `AH=05h` -> limpar estado local de notificação

O objetivo seria permitir que clientes DOS consultem o TSR sem implementar rede.

Essa interface não deve substituir o protocolo de rede.
Ela serve para comunicação local entre aplicações DOS e o residente.

## Próximos passos

- mapear integrações com `INT 2Fh`
- avaliar `INT 60h` como interface futura
- documentar estratégia de polling
- definir limites de memória e de execução

## Status de implementação

- documentação inicial
- sem implementação funcional ainda

## Boas práticas para exemplos de código

- exemplos devem ser curtos
- exemplos devem ser próprios do RetroNet
- exemplos devem explicar o conceito
- exemplos não devem copiar implementações completas de terceiros
- exemplos devem priorizar clareza e compatibilidade
- exemplos Assembly devem ser marcados como conceituais
- exemplos não devem ser tratados como implementação final

## Relação com outros documentos

- [Packet Driver](packet-driver.md)
- [Notificações](notifications.md)
- [DOS](dos.md)
- [Áudio](audio.md)
