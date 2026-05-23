# RNTSR — RetroNet TSR Agent

## Status

Pesquisa / arquitetura inicial.

## Objetivo

Explicar que o RNTSR é um agente residente para MS-DOS e FreeDOS
responsável apenas por:

- heartbeat
- polling leve
- consulta de eventos pendentes
- notificação temporária
- estado local mínimo

O RNTSR não é um cliente completo.

## O que o RNTSR faz

- verifica sessão
- consulta eventos
- exibe popup temporário
- pode tocar beep futuramente
- mantém estado mínimo
- pode expor interface local futura para clientes DOS

## O que o RNTSR não faz

- não baixa mensagens completas
- não baixa boards
- não processa QWK
- não baixa anexos
- não transfere arquivos
- não substitui RNMAIL, RNBOARD, RNCHAT ou RNMSG

## Fases do RNTSR

O RNTSR deve ser pensado em fases separadas:

1. instalação
2. código residente
3. código descartável após instalação
4. possível rotina futura de remoção

O código de instalação não deve permanecer em memória se não for
necessário.

## Pré-condições de instalação

Antes de ficar residente, o agente deve verificar:

- se já existe instância carregada
- se há Packet Driver disponível
- se a configuração mínima está válida
- se o ambiente permite rede
- se a interrupção ou API local pretendida está livre ou é segura
- se o modo de operação foi configurado corretamente

Se qualquer pré-condição falhar, o RNTSR deve abortar sem ficar
residente.

Sem Packet Driver, não carregar TSR.

## Evitar TSR zumbi

O RNTSR não deve permanecer em memória se não houver rede funcional,
configuração mínima ou condição operacional válida.

## Cuidados de arquitetura

- evitar chamadas DOS pesadas dentro de interrupções
- evitar reentrância indevida em DOS
- preservar registradores
- preservar flags
- preservar contexto da aplicação interrompida
- preservar stack ou trocar stack de forma segura quando necessário
- manter estado mínimo
- usar flags internas para adiar trabalho pesado
- não bloquear a máquina esperando rede
- não executar polling durante desenho do popup
- não tocar áudio longo em rotina crítica
- manter código pequeno e previsível

## Permanência residente

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

INT 21h/AH=31h deve ser o caminho preferencial para novos TSRs.

INT 27h é legado e deve ser evitado salvo necessidade específica de
compatibilidade.

O tamanho residente precisa ser calculado com cuidado.
O código descartável não deve ficar residente.
O cálculo deve considerar PSP, segmentos e final real do código
residente.

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

Não implementar ainda.

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

- detectar Packet Driver
- abortar sem ficar residente se não houver rede
- instalar TSR mínimo
- manter estado local mínimo
- executar polling leve
- exibir popup em modo texto
- suportar `STATUS`, `POLLNOW` e `QUIET`

## Próximos passos

- mapear integrações com `INT 2Fh`
- avaliar `INT 60h` como interface futura
- documentar estratégia de polling
- definir limites de memória e de execução

## Status de implementação

- documentação inicial
- sem implementação funcional ainda

## Relação com outros documentos

- [Packet Driver](packet-driver.md)
- [Notificações](notifications.md)
- [DOS](dos.md)
- [Áudio](audio.md)
