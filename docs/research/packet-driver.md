# Packet Driver no RetroNet

## Status

Pesquisa / arquitetura inicial.

## Objetivo

Documentar o Packet Driver como pré-condição para o RNTSR em ambiente
DOS.

## Por que Packet Driver

- compatibilidade com MS-DOS
- integração com placas Ethernet antigas
- ecossistema já conhecido por softwares TCP/IP DOS
- compatível com mTCP e outras stacks

## Regra principal

O RNTSR só deve carregar se o Packet Driver estiver carregado.

Sem Packet Driver, ele não deve permanecer residente.

Isso evita TSR zumbi sem rede e reduz consumo de memória em sistemas
que não estão prontos para operar.

## Ordem recomendada no AUTOEXEC.BAT

```bat
NE2000.COM 0x60 5 0x300
RNTSR.EXE /LOAD
```

## Detecção conceitual

Uma estratégia inicial de detecção pode incluir:

- varrer interrupções prováveis
- validar assinatura ou serviço do Packet Driver
- confirmar capacidade mínima de rede

A detecção deve acontecer antes da instalação residente.
Somente depois disso o RNTSR pode ser carregado com segurança.

## Estratégias candidatas

- interrupção configurável em `RETRONET.CFG`
- varredura de vetores prováveis
- integração com mTCP
- testes com NE2000
- fallback para não carregar o TSR

## Falha esperada

```text
RetroNet TSR Agent
Packet Driver não encontrado.
Carregue o Packet Driver antes de executar RNTSR /LOAD.
RNTSR não foi carregado.
```

## Próximos estudos

- integração com mTCP
- Packet Driver em DOSBox-X
- Packet Driver em 86Box
- Packet Driver em PCem
- Packet Driver em QEMU
- testes com NE2000
- testes em hardware real

## Relação com outros documentos

- [RNTSR](tsr.md)
- [Notificações](notifications.md)
- [Networking](networking.md)
