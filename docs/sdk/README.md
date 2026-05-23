# RetroNet SDK

O RetroNet SDK é o kit de desenvolvimento e integração do ecossistema
RetroNet.

Ele existe para preservar, reaproveitar e reintegrar software legado,
compiladores, bibliotecas, protocolos e conhecimento técnico ainda útil.

## Objetivo

- dar base para aplicações retro e clientes leves
- facilitar integração com software legado
- manter compatibilidade acima de modernização cosmética
- suportar redes lentas e máquinas limitadas
- oferecer uma plataforma, não apenas um cliente

## Status

Esta documentação descreve a visão e a arquitetura inicial do SDK.
Ainda não é uma API completa nem um pacote final de distribuição.

## Leitura recomendada

- [Visão](vision.md)
- [Arquitetura](architecture.md)
- [Compatibilidade](compatibility.md)
- [Linguagens](languages.md)
- [Toolchains](toolchains.md)
- [Protocolo](protocol.md)
- [Capabilities](capabilities.md)
- [Clientes](clients.md)
- [Áudio](audio.md)
- [Vídeo/UI](video.md)
- [Networking](networking.md)
- [Referências](references.md)
- [Roadmap](roadmap.md)

## Estrutura sugerida

```text
RetroNet SDK/
├── include/
├── lib/
├── src/
├── examples/
├── tools/
├── bindings/
│   ├── c/
│   ├── asm/
│   ├── pascal/
│   └── cpp/
├── docs/
└── research/
```

## Próximos passos

- consolidar interfaces públicas
- definir pacote mínimo para cliente CLI/TUI
- definir bindings Pascal e C++
- mapear toolchains DOS e builds reproduzíveis
- validar integração com clientes, TSRs e gateways
