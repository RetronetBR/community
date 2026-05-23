# Compatibilidade

## Status

Pesquisa / arquitetura inicial.

Esta página define diretrizes de compatibilidade para o RetroNet SDK.

Ela não afirma suporte já validado nas plataformas listadas.

## Objetivo

Documentar compatibilidade entre versões de DOS e ambientes de
execução.

## Regras

- priorizar binários pequenos
- evitar dependências pesadas
- evitar runtime moderno desnecessário
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

## Plataformas consideradas

- MS-DOS 2.x
- MS-DOS 3.x
- MS-DOS 5.x
- MS-DOS 6.x
- PC-DOS
- FreeDOS
- DOSBox-X
- 86Box
- PCem
- QEMU
- hardware real

## Matriz futura de compatibilidade

Esta tabela é conceitual.

Ela não representa validação concluída.

| Recurso | MS-DOS 2.x | MS-DOS 3.x | MS-DOS 5.x | MS-DOS 6.x | FreeDOS | DOSBox-X | 86Box | PCem | QEMU | Hardware real |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| RNTSR load | pesquisa | planejado | planejado | planejado | planejado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| RNTSR unload | pesquisa | planejado | planejado | planejado | planejado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| Packet Driver | depende de driver | depende de driver | depende de driver | depende de driver | depende de driver | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| Popup texto | planejado | planejado | planejado | planejado | planejado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| Beep | planejado | planejado | planejado | planejado | planejado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| mTCP | não testado | não testado | não testado | não testado | não testado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| Cliente CLI | planejado | planejado | planejado | planejado | planejado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| Cliente TUI | planejado | planejado | planejado | planejado | planejado | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| Cliente VGA | alvo futuro | alvo futuro | alvo futuro | alvo futuro | alvo futuro | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| QWK | alvo futuro | alvo futuro | alvo futuro | alvo futuro | alvo futuro | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |
| IPX | alvo futuro | alvo futuro | alvo futuro | alvo futuro | alvo futuro | pesquisa | pesquisa | pesquisa | pesquisa | pesquisa |

## Uso futuro

- decidir quais recursos são obrigatórios
- decidir quais recursos são opcionais
- definir degradação graciosa por capability
- manter clientes antigos operando com subconjunto funcional
- criar matriz de compatibilidade validada por testes

## Próximos passos

- documentar perfis mínimos por plataforma
- definir limites de memória por alvo
- definir recursos proibidos em clientes base
- separar compatibilidade por versão de DOS
- separar compatibilidade por ambiente de execução
- validar comportamento em emuladores
- validar comportamento em hardware real
