# Toolchains

## Status

Pesquisa / arquitetura inicial.

Esta página documenta toolchains candidatas para o RetroNet SDK.

Nem todas as toolchains serão suportadas em todos os módulos.

## Objetivo

Documentar caminhos de compilação compatíveis com o ecossistema retro.

## Linguagens

- C
- Assembly
- Pascal
- C++ futuramente, quando fizer sentido

## Toolchains alvo

- Turbo C
- Borland C
- Microsoft C legado
- OpenWatcom
- DJGPP
- Turbo Pascal
- Borland Pascal
- NASM
- MASM
- TASM

## Compiladores modernos

Compiladores modernos que geram binários DOS também podem ser
avaliados.

Formatos relevantes:

- `.COM`
- `.EXE`
- binários para DOS extender
- builds cruzados gerados fora do DOS

## Uso recomendado

- Assembly para TSR
- Assembly para interrupções
- Assembly para rotinas críticas
- Assembly para acesso de baixo nível
- C para núcleo do SDK
- C para bibliotecas
- C para clientes portáveis
- Pascal para clientes retro
- Pascal para bindings voltados à comunidade retro
- C++ apenas quando não prejudicar compatibilidade

## Uso por alvo

- real mode 16-bit
- DOS extender
- emulador
- hardware real

## Exemplos por alvo

Real mode 16-bit:

- Turbo C
- Borland C
- Turbo Pascal
- Borland Pascal
- MASM
- TASM
- NASM

DOS extender:

- DJGPP
- OpenWatcom

Emulador:

- QEMU
- DOSBox-X
- PCem
- 86Box
- builds modernos para DOS

Hardware real:

- toolchains pequenas
- binários previsíveis
- rotinas compatíveis com o alvo escolhido

## Integração entre linguagens

O SDK pode ter núcleo em C.

Rotinas críticas podem usar Assembly.

Bindings Pascal podem expor uma API simples.

Clientes DOS podem ser escritos em C ou Pascal.

O RNTSR provavelmente terá partes em Assembly.

O objetivo é permitir contribuição de diferentes perfis de
programadores.

## Uso futuro

- builds de cliente fora do DOS
- geração de artefatos DOS
- CI/CD para builds DOS
- artefatos reproduzíveis
- exemplos mínimos por linguagem
- exemplos mínimos por plataforma

## Próximos passos

- definir versões mínimas por toolchain
- classificar toolchains por target
- documentar flags essenciais
- documentar limitações essenciais
- separar exemplos por alvo
- separar exemplos por linguagem
