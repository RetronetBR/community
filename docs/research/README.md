# Research

## Objetivo

Reunir trilhas de pesquisa para apoiar o desenvolvimento do RetroNet SDK e do kit de desenvolvimento.

## Status

Esta pasta não documenta funcionalidades prontas.
Ela reúne referências técnicas, históricas e práticas para orientar decisões futuras.

## Documentos

- [TSR / RNTSR](tsr.md): agente residente mínimo, polling leve e integração local por interrupção.
- [Packet Driver](packet-driver.md): pré-condição de rede para o RNTSR em ambientes DOS.
- [Notificações](notifications.md): popup temporário, estado mínimo e comportamento seguro em modo texto.
- [DOS](dos.md): diretrizes para clientes, TSRs e compatibilidade com MS-DOS e FreeDOS.
- [Pascal](pascal.md): base para bindings e clientes retro com Turbo Pascal e Borland Pascal.
- [Otimização de C](c-optimization.md): práticas para código pequeno, rápido e previsível.
- [Vídeo VGA](video-vga.md): trilha para UI em modo texto e VGA.
- [Áudio](audio.md): alertas sonoros e síntese retro.
- [Networking](networking.md): transportes legados e caminhos prováveis para o SDK.
- [IPX](ipx.md): ponte futura para compatibilidade com redes e software legado.
- [Referências do SDK](../sdk/references.md): lista técnica de materiais e referências úteis para o SDK.

## Referências iniciais

### 1. IPXBox
- Link: https://github.com/fragglet/ipxbox
- Tema: IPX sobre redes modernas
- Descrição: projeto para comunicação IPX e compatibilidade com softwares e jogos legados
- Uso no RetroNet SDK: possível base para `rn-ipx`, gateway IPX e compatibilidade com redes DOS e jogos antigos

### 2. Vídeo demonstrativo IPXBox
- Link: https://youtu.be/5VeVaFbORhI
- Tema: demonstração de uso do IPXBox
- Descrição: vídeo prático de integração com IPXBox
- Uso no RetroNet SDK: referência prática para integração e testes de rede IPX

### 3. Desenvolvimento TSR
- Link: https://youtu.be/KMUT9TEoe4Y
- Tema: desenvolvimento de TSR em MS-DOS
- Descrição: referência para TSR residente, polling leve e popup de notificações
- Uso no RetroNet SDK: base para `RNTSR`, polling, popup de notificações e integração com `INT 2Fh`

### 4. ChillRuns
- Link: https://youtube.com/@chillruns6221
- Tema: programação MS-DOS
- Descrição: conteúdo sobre DOS e desenvolvimento retro
- Uso no RetroNet SDK: referência para desenvolvimento de clientes DOS, memória, interrupções e aplicações retro

### 5. root42
- Link: https://youtube.com/@root42
- Tema: retrocomputação, DOS, hardware e low-level
- Descrição: conteúdo técnico com foco em ambiente retro
- Uso no RetroNet SDK: referência para programação próxima ao hardware e preservação retro

### 6. NCOT Tech
- Link: https://youtube.com/@ncot_tech
- Tema: retrocomputação, hardware e desenvolvimento técnico
- Descrição: conteúdo complementar sobre hardware e software retro
- Uso no RetroNet SDK: referência complementar para hardware retro e ecossistema técnico

### 7. Please and Thank You - VGA
- Link: https://youtube.com/@pleaseandthankyou-vga
- Tema: VGA e retrocomputação
- Descrição: conteúdo focado em vídeo e interfaces VGA
- Uso no RetroNet SDK: referência para futuras interfaces gráficas, `librnui` e manipulação VGA

### 8. sudo cpp
- Link: https://youtube.com/@sudocpp
- Tema: C/C++, desenvolvimento e programação low-level
- Descrição: conteúdo técnico sobre C/C++ e baixo nível
- Uso no RetroNet SDK: referência para engenharia do SDK e boas práticas em C/C++

### 9. Pascal/MS-DOS
- Link: https://youtu.be/h8OdxRemIEo
- Tema: Pascal e programação MS-DOS
- Descrição: referência para Pascal em ambiente DOS
- Uso no RetroNet SDK: referência para bindings Pascal, exemplos em Turbo Pascal/Borland Pascal e possível uso com Turbo Vision

### 10. FreeDOS Project
- Link: https://youtube.com/@freedosproject
- Tema: FreeDOS e ecossistema DOS moderno
- Descrição: referência para sistemas compatíveis com DOS
- Uso no RetroNet SDK: plataforma de testes, compatibilidade DOS e referência de preservação ativa

### 11. Otimização de código C
- Link: https://youtu.be/eTf_Voy1-ew
- Tema: otimização de código C
- Descrição: conteúdo sobre redução de custo e melhora de desempenho
- Uso no RetroNet SDK: reduzir memória, melhorar desempenho, criar TSRs menores e clientes eficientes

### 12. Compiladores modernos para DOS/.COM
- Link: https://youtu.be/E1IdWPj34Zw
- Tema: compiladores modernos gerando binários DOS, incluindo `.COM`
- Descrição: referência para toolchains modernas com saída retro
- Uso no RetroNet SDK: builds reproduzíveis, CI/CD e toolchain moderna para clientes DOS

### 13. VGA framebuffer com NASM/QEMU
- Link: https://youtu.be/JhkkyQOtqkg
- Tema: framebuffer VGA, NASM e QEMU
- Descrição: referência para gráficos e testes em emulador
- Uso no RetroNet SDK: base para interfaces gráficas DOS, `librnui` e testes em emulador

### 14. Peek/Poke e VGA memory mapped
- Link: https://youtu.be/DH2h2paRC4U
- Tema: acesso direto à memória, `peek`/`poke` e gráficos VGA em DOS
- Descrição: referência para manipulação direta de vídeo
- Uso no RetroNet SDK: popups, widgets, interfaces gráficas simples e manipulação direta de vídeo

### 15. Programação AdLib/OPL
- Link: https://youtu.be/XKy1cE4EYMk
- Tema: programação da placa AdLib/OPL
- Descrição: referência para áudio OPL e síntese simples
- Uso no RetroNet SDK: alertas sonoros, `librnaudio`, notificações do `RNTSR` e áudio retro

### 16. Sound Blaster 16
- Link: não informado
- Tema: Sound Blaster 16
- Descrição: ainda falta referência consolidada
- Uso no RetroNet SDK: futura trilha para áudio PCM, WAV, DMA, IRQ, mixer e alertas customizados

## Relação com a documentação do SDK

- [RetroNet SDK](../sdk/README.md)
- [Visão do SDK](../sdk/vision.md)
- [Capabilities do SDK](../sdk/capabilities.md)
- [Networking do SDK](../sdk/networking.md)

## Próximos passos

- priorizar tópicos mais úteis para DOS e clientes leves
- manter ligação com a documentação do SDK
