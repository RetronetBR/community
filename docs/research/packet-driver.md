# Packet Driver no RetroNet

## Status

Pesquisa / arquitetura inicial.

Não há implementação funcional documentada neste arquivo.

## Objetivo

Documentar o Packet Driver como pré-condição para o RNTSR em ambiente
DOS.

## Regra principal

Sem Packet Driver detectado, o RNTSR não deve carregar residente.

Essa regra existe para evitar um TSR sem rede funcional consumindo
memória convencional.

## Por que Packet Driver

Packet Driver é uma base provável para conectividade TCP/IP em DOS.

Ele é relevante porque oferece:

- compatibilidade com MS-DOS
- compatibilidade com FreeDOS
- integração com placas Ethernet antigas
- ecossistema conhecido por softwares TCP/IP DOS
- possível integração com mTCP
- caminho de teste em emuladores
- caminho de teste em hardware real

## Ordem recomendada no AUTOEXEC.BAT

Exemplo conceitual:

```bat
NE2000.COM 0x60 5 0x300
RNTSR.EXE /LOAD
```

## Detecção conceitual

A detecção deve acontecer antes da instalação residente.

Estratégia conceitual:

- detectar interrupção do Packet Driver
- validar assinatura ou serviço
- confirmar capacidade mínima de rede
- validar configuração local
- somente então instalar residente
- abortar se a rede não estiver disponível

## Estratégias candidatas

- interrupção configurável em `RETRONET.CFG`
- varredura de vetores prováveis
- integração com mTCP
- testes com NE2000
- fallback para não carregar o TSR

## Falha esperada

Mensagem conceitual:

```text
RetroNet TSR Agent
Packet Driver não encontrado.
Carregue o Packet Driver antes de executar RNTSR /LOAD.
RNTSR não foi carregado.
```

## Comportamento esperado

Se a detecção falhar, o RNTSR deve encerrar sem ficar residente.

Se a configuração estiver incompleta, o RNTSR deve encerrar sem ficar
residente.

Se a rede não estiver disponível, o RNTSR deve encerrar sem ficar
residente.

## Próximos estudos

- integração com mTCP
- Packet Driver em DOSBox-X
- Packet Driver em 86Box
- Packet Driver em PCem
- Packet Driver em QEMU
- testes com NE2000
- testes em hardware real
- estratégia de configuração por `RETRONET.CFG`

## Relação com outros documentos

- [RNTSR](tsr.md)
- [Notificações](notifications.md)
- [Networking](networking.md)
