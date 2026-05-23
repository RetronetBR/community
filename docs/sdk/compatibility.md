# Compatibilidade

## Objetivo

Manter compatibilidade acima de modernização.

## Regras

- priorizar binários pequenos
- evitar dependências pesadas
- evitar recursos que exijam runtime moderno desnecessário
- preservar clientes antigos sempre que possível
- não quebrar o contrato do protocolo sem versão explícita

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
- definir degradacão graciosa por capability
- manter clientes antigos operando com subconjunto funcional

## Próximos passos

- documentar perfis mínimos por plataforma
- definir limites de memória por alvo
- definir recursos proibidos em clientes base
