# Arquitetura

## Objetivo

Organizar o SDK em camadas pequenas, reutilizáveis e compatíveis com ambientes retro.

## Camadas previstas

- `librn`: núcleo do SDK
- `librnui`: interfaces texto, TUI e VGA
- `librnaudio`: alertas sonoros e áudio retro
- `rn-auth`: autenticação
- `rn-tsr`: TSR residente
- `rn-msg`: mensagens
- `rn-mail`: leitor de email/mensagens
- `rn-board`: boards e fóruns
- `rn-qwk`: integração QWK
- `rn-ftp`: integração com arquivos
- `rn-ipx`: ponte futura com IPX/IPXBox

## Fluxo conceitual

```text
aplicação
  -> librn
  -> transporte/capabilities
  -> servidor RetroNet
  -> roteamento/federação
```

## Status

Esses componentes são previstos. A documentação define o mapa arquitetural para orientar implementação e contribuições.

## Uso futuro

- permitir clientes simples em C e Pascal
- permitir reutilização em DOS e Windows antigos
- expor interfaces mínimas para UI, áudio e rede

## Próximos passos

- separar APIs por domínio
- definir cabeçalhos públicos
- definir formato mínimo das mensagens da biblioteca
- documentar dependências entre módulos
