# RNTSR — RetroNet TSR Agent

## Status

Pesquisa / arquitetura inicial.

O RNTSR ainda não possui implementação funcional documentada neste
repositório.

Este arquivo descreve a arquitetura pretendida, os cuidados técnicos e
os limites do agente residente.

## Objetivo

O RNTSR é um agente residente mínimo para MS-DOS e FreeDOS.

Ele deve consultar periodicamente o servidor RetroNet, detectar eventos
pendentes e avisar o usuário com notificações temporárias.

O RNTSR não é um cliente completo de mensagens.

## Escopo do agente

O RNTSR deve ser responsável apenas por tarefas leves.

- heartbeat
- polling leve
- consulta de eventos pendentes
- notificação temporária
- estado local mínimo
- interface local futura para clientes DOS

## O que o RNTSR faz

- verifica sessão
- consulta eventos
- detecta novidades
- exibe popup temporário
- pode tocar beep futuramente
- mantém estado local mínimo
- pode expor uma API local para clientes DOS

## O que o RNTSR não faz

- não baixa mensagens completas
- não baixa boards
- não processa QWK
- não baixa anexos
- não transfere arquivos
- não substitui RNMAIL
- não substitui RNBOARD
- não substitui RNCHAT
- não substitui RNMSG

## Fases do RNTSR

O RNTSR deve ser dividido em fases claras.

1. fase de instalação
2. código residente
3. código descartável após instalação
4. possível rotina futura de remoção

O código de instalação não deve permanecer em memória se não for
necessário.

## Pré-condições de instalação

Antes de ficar residente, o RNTSR deve validar o ambiente.

- verificar se já existe instância carregada
- verificar se há Packet Driver disponível
- verificar se a configuração mínima está válida
- verificar se o ambiente permite rede
- verificar se a interrupção local pretendida está livre ou segura
- verificar se o modo de operação foi configurado corretamente

Se qualquer pré-condição falhar, o RNTSR deve abortar sem ficar
residente.

Sem Packet Driver, não carregar TSR.

## Evitar TSR zumbi

O RNTSR não deve permanecer em memória sem rede funcional.

O RNTSR também não deve permanecer em memória se a configuração mínima
estiver ausente ou inválida.

Esse comportamento evita um residente sem utilidade consumindo memória
convencional.

## Cuidados de arquitetura

- evitar chamadas DOS pesadas dentro de interrupções
- evitar reentrância indevida em DOS
- preservar registradores
- preservar flags
- preservar o contexto da aplicação interrompida
- preservar stack ou trocar stack de forma segura quando necessário
- manter estado mínimo
- usar flags internas para adiar trabalho pesado
- não bloquear a máquina esperando rede
- não executar polling durante desenho do popup
- não tocar áudio longo em rotina crítica
- manter código pequeno e previsível
- permitir modo quiet/silent
- permitir desativar popup sem descarregar o agente

## Separação de código

A implementação futura deve separar o código por responsabilidade.

- código de instalação
- código residente
- código descartável após instalação
- código de interface local
- código de remoção futura

Essa separação é importante porque apenas a parte realmente necessária
deve permanecer residente.

## Temas técnicos para estudo

Implementações futuras devem estudar os seguintes pontos.

- Program Segment Prefix
- cálculo de memória residente
- DOS Keep Process
- preservação de vetores de interrupção
- restauração de vetores de interrupção
- detecção de instância já instalada
- remoção segura
- troca de stack
- preservação de stack
- cuidado com chamadas DOS a partir de interrupções

## Permanência residente

INT 21h/AH=31h deve ser o caminho preferencial para novos TSRs.

INT 27h é legado e deve ser evitado, salvo necessidade específica de
compatibilidade.

O tamanho residente precisa ser calculado com cuidado.

O cálculo deve considerar PSP, segmentos e final real do código
residente.

O código descartável não deve ficar residente.

Exemplo conceitual de Assembly:

```asm
; RNTSR - exemplo conceitual
; Mantém residente apenas a parte necessária.
; AH=31h: DOS Keep Process
; DX: tamanho residente em parágrafos
;
; Este exemplo não é implementação final.

mov ah, 31h
mov dx, resident_size_paragraphs
int 21h
```

## Fluxo de inicialização

Fluxo conceitual:

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

## Interface futura por interrupção

O RNTSR poderá expor uma interrupção própria para consulta local por
clientes DOS.

Essa interface não substitui o protocolo RetroNet de rede.

Ela serve apenas para comunicação local entre programas DOS e o
residente.

Exemplo conceitual:

```asm
; Consulta local ao RNTSR
; Exemplo conceitual, não implementação final.

mov ah, 00h
int 60h
jc rn_error
```

Funções conceituais futuras:

- `AH=00h` -> status do agente
- `AH=01h` -> quantidade de eventos pendentes
- `AH=02h` -> forçar poll
- `AH=03h` -> alternar quiet mode
- `AH=04h` -> consultar última notificação
- `AH=05h` -> limpar estado local de notificação

## Detecção de instância já carregada

A implementação futura deve prever uma forma segura de detectar se o
RNTSR já está instalado.

Possibilidades conceituais:

- assinatura em área residente
- chamada por interrupção local
- checagem de vetor
- resposta de identificação
- versão do agente
- capabilities locais

Nenhuma dessas alternativas está implementada neste momento.

## Remoção futura

`RNTSR /UNLOAD` é desejável, mas deve ser tratado com cuidado.

A remoção só deve ocorrer se:

- os vetores originais puderem ser restaurados
- nenhuma aplicação estiver usando o agente
- a cadeia de interrupções estiver íntegra
- o bloco de memória puder ser liberado com segurança

## Modelo de polling

Valores conceituais:

- Idle: 120s
- Ativo: 30s
- Erro: backoff 5min, 10min, 30min
- Manual: `RNTSR /POLLNOW`

O polling deve ser curto, não bloqueante e com timeout.

## Eventos previstos

- `MAIL`
- `BOARD`
- `CHAT`
- `SYSTEM`
- `MOTD`
- `FILE`
- `USER`

## Capabilities relacionadas

- `RN_CAP_NOTIFY`
- `RN_CAP_AUDIO`
- `RN_CAP_MOTD`

## Comandos previstos

- `RNTSR /LOAD`
- `RNTSR /UNLOAD`
- `RNTSR /STATUS`
- `RNTSR /POLLNOW`
- `RNTSR /QUIET`
- `RNTSR /POPUP:ON`
- `RNTSR /POPUP:OFF`

## Boas práticas para exemplos de código

- exemplos devem ser curtos
- exemplos devem ser próprios do RetroNet
- exemplos devem explicar o conceito
- exemplos não devem copiar implementações completas de terceiros
- exemplos devem priorizar clareza e compatibilidade
- exemplos Assembly devem ser marcados como conceituais
- exemplos não devem ser tratados como implementação final

## MVP RNTSR v0.1

Escopo conceitual do primeiro MVP:

- detectar Packet Driver
- abortar sem ficar residente se não houver rede
- instalar TSR mínimo
- manter estado local mínimo
- executar polling leve
- exibir popup em modo texto
- suportar `STATUS`
- suportar `POLLNOW`
- suportar `QUIET`

## Próximos passos

- mapear integrações com `INT 2Fh`
- avaliar `INT 60h` como interface futura
- documentar estratégia de polling
- definir limites de memória e de execução
- definir estratégia segura de unload
- validar comportamento em emuladores
- validar comportamento em hardware real

## Status de implementação

- documentação inicial
- sem implementação funcional ainda

## Relação com outros documentos

- [Packet Driver](packet-driver.md)
- [Notificações](notifications.md)
- [DOS](dos.md)
- [Áudio](audio.md)
