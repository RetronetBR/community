# DOS

## Status

MS-DOS e FreeDOS são plataformas-alvo centrais para o SDK,
especialmente para clientes simples, TSRs e interfaces texto.

## Objetivo

Visão geral de desenvolvimento DOS para o RetroNet SDK.

## Visão geral

Programas para RetroNet em DOS devem ser pequenos, modulares e
previsíveis.

A separação entre cliente e agente residente deve ser clara.

O SDK deve favorecer compatibilidade acima de recursos avançados e
priorizar chamadas DOS documentadas sempre que possível.

## Cuidados

- diferenças entre versões de DOS importam
- `INT 21h` deve ser tratado com cuidado em contexto residente ou de
  interrupção
- `Assembly` deve ser usado apenas onde necessário
- `C` e `Pascal` devem ser preferidos para clientes quando fizer sentido
- o código deve ser separado por nível de dependência:
  - portátil
  - dependente de DOS
  - dependente de hardware
  - dependente de Packet Driver
  - dependente de UI
  - dependente de áudio

## Diretrizes

- pensar primeiro em máquinas lentas
- evitar alocação dinâmica desnecessária
- evitar dependência de DOS moderno
- manter fallback textual
- priorizar arquivos simples e formatos legíveis
- preservar compatibilidade com emuladores e hardware real

## Separação de camadas

- código portátil
- código dependente de DOS
- código dependente de hardware
- código dependente de Packet Driver
- código dependente de UI
- código dependente de áudio

## Uso futuro

- definir limites de memória
- mapear interrupções úteis
- orientar builds `.COM` e `.EXE`
- documentar diferenças entre MS-DOS, PC-DOS e FreeDOS
- documentar comportamento esperado em ambientes emulados

## Próximos passos

- documentar APIs e convenções mais úteis para clientes RetroNet
- mapear peculiaridades de FreeDOS e DOS emulável
