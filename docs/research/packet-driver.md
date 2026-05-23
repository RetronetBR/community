# Packet Driver no RetroNet

## Objetivo

Documentar o Packet Driver como pré-condição para o RNTSR em ambiente DOS.

## Status

Pesquisa / arquitetura inicial.

## Por que Packet Driver

- compatibilidade com MS-DOS
- integração com placas Ethernet antigas
- ecossistema já conhecido por softwares TCP/IP DOS
- compatível com mTCP e outras stacks

## Regra do RNTSR

O RNTSR só deve carregar se o Packet Driver estiver carregado.
Sem Packet Driver, ele não deve permanecer residente.

Isso evita TSR zumbi sem rede e reduz consumo de memória em sistemas que não estão prontos para operar.

## Exemplo de AUTOEXEC.BAT

    NE2000.COM 0x60 5 0x300
    RNTSR.EXE /LOAD

## Detecção

Uma estratégia inicial de detecção pode incluir:

- varrer interrupções prováveis
- validar assinatura ou serviço do Packet Driver
- confirmar capacidade mínima de rede

A detecção deve acontecer antes da instalação residente.
Somente depois disso o RNTSR pode ser carregado com segurança.

## Ordem recomendada no AUTOEXEC.BAT

    NE2000.COM 0x60 5 0x300
    RNTSR.EXE /LOAD

## Falha esperada

```text
RetroNet TSR Agent
Packet Driver não encontrado.
Carregue o Packet Driver antes de executar RNTSR /LOAD.
RNTSR não foi carregado.
```

## Próximos passos

- definir estratégia exata de detecção
- integrar a detecção com a fase de instalação do RNTSR
- integrar com mTCP
- testar em DOSBox-X
- testar em 86Box
- testar em PCem
- testar em QEMU
- testar com NE2000
- testar em hardware real

## Relação com outros documentos

- [RNTSR](tsr.md)
- [Notificações](notifications.md)
- [Networking](networking.md)
