# Compatibilidade

## Objetivo

Manter compatibilidade acima de modernização.

## Regras

- priorizar binários pequenos
- evitar dependências pesadas
- evitar recursos que exijam runtime moderno desnecessário
- preservar clientes antigos sempre que possível
- não quebrar o contrato do protocolo sem versão explícita
- tratar compatibilidade entre versões do DOS como requisito do SDK
- evitar recursos que limitem o cliente a uma única versão
- separar chamadas por nível de compatibilidade
- documentar quando uma função depende de DOS 2.x, 3.x ou superior
- considerar MS-DOS, PC-DOS, FreeDOS e ambientes emulados

## Diretrizes

- evitar dependência de uma única versão de DOS
- separar chamadas por nível de compatibilidade
- documentar quando algo depende de DOS 2.x, 3.x ou superior
- considerar MS-DOS, PC-DOS, FreeDOS e ambientes emulados
- criar futuramente matriz de compatibilidade

## Plataformas

- MS-DOS
- FreeDOS
- Windows 3.x
- Windows 9x
- Windows 2000
- Linux
- emuladores
- hardware real

## Status

A compatibilidade é uma diretriz do projeto, não uma promessa de suporte imediato para todas as plataformas.

## Uso futuro

- decidir quais recursos são obrigatórios e quais são opcionais
- definir degradação graciosa por capability
- manter clientes antigos operando com subconjunto funcional
- criar uma matriz futura de compatibilidade

## Matriz futura de compatibilidade

Colunas sugeridas:

- Recurso
- MS-DOS 2.x
- MS-DOS 3.x
- MS-DOS 5.x
- MS-DOS 6.x
- FreeDOS
- DOSBox-X
- 86Box
- PCem
- QEMU
- Hardware real

Linhas sugeridas:

- RNTSR load
- RNTSR unload
- Packet Driver
- Popup texto
- Beep
- mTCP
- Cliente CLI
- Cliente TUI
- Cliente VGA
- QWK
- IPX

## Próximos passos

- documentar perfis mínimos por plataforma
- definir limites de memória por alvo
- definir recursos proibidos em clientes base
- separar compatibilidade por versão de DOS e por ambiente de execução
