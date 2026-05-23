# Toolchains

## Objetivo

Documentar caminhos de compilação compatíveis com o ecossistema retro.

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
- compiladores modernos gerando binários DOS e `.COM`

## Uso por alvo

- real mode 16-bit
- DOS extender
- emulador
- hardware real

## Uso recomendado

- Assembly para TSR, interrupções, rotinas críticas e baixo nível
- C para bibliotecas e clientes portáveis
- Pascal para clientes e bindings voltados à comunidade retro
- C++ apenas onde não prejudicar compatibilidade

## Exemplos por alvo

- real mode 16-bit: Turbo C, Borland C, Turbo Pascal, Borland Pascal, MASM/TASM, NASM
- DOS extender: DJGPP, OpenWatcom
- emulador: compilações modernas para DOS/.COM/.EXE e testes em QEMU, DOSBox-X, PCem e 86Box
- hardware real: toolchains pequenas, binários previsíveis e rotinas
  compatíveis com o alvo escolhido

## Integração entre linguagens

- o SDK pode ter núcleo em C
- rotinas críticas podem usar Assembly
- bindings Pascal podem expor API simples
- clientes DOS podem ser escritos em C ou Pascal
- RNTSR provavelmente terá partes em Assembly
- o objetivo é permitir contribuição de diferentes perfis de programadores

## Uso futuro

- builds de cliente fora do DOS para gerar artefatos DOS
- CI/CD para builds DOS
- artefatos reproduzíveis

## Status

Esta é uma trilha de compatibilidade e pesquisa.

Nem todas as toolchains serão suportadas em todos os módulos.

## Próximos passos

- definir versões mínimas por toolchain
- classificar toolchains por target
- documentar flags e limitações essenciais
- separar exemplos por alvo e linguagem
