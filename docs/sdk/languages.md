# Linguagens

## Objetivo

Definir as linguagens alvo do SDK e os motivos dessa escolha.

## Linguagens alvo

- C
- Assembly
- Pascal
- C++
- futuramente FreePascal e Delphi quando fizer sentido

## Por que Pascal importa

Pascal é relevante no meio retro porque há muitos programadores com
experiência em Turbo Pascal, Borland Pascal e Delphi inicial.

Isso amplia a base de colaboradores e facilita a criação de clientes e
ferramentas para DOS e ambientes antigos.

## Status

A lista de linguagens é uma orientação de ecossistema, não uma
obrigatoriedade para todos os módulos.

## Uso futuro

- `C` para núcleo e clientes portáveis
- `Assembly` para otimizações, TSRs e acesso direto a hardware
- `Pascal` para clientes e ferramentas retro
- `C++` para integrações e componentes específicos quando valer o custo

## Próximos passos

- mapear binding mínimo por linguagem
- publicar exemplos conceituais
- evitar APIs que dificultem compilação em toolchains antigas
