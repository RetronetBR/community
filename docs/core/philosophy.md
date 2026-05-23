# Filosofia

> O usuário envia uma mensagem. A rede entrega.

RetroNet foi desenhada para manter a lógica da rede simples e previsível.

## Princípios

- store-and-forward
- simplicidade
- compatibilidade retro
- cliente simples
- rede inteligente
- sem dependências pesadas
- sem JSON
- sem TLS obrigatório
- sem criptografia obrigatória
- segurança opcional via PGP externo

RetroNet não é um sistema criptográfico. O projeto transporta, roteia e entrega mensagens.
Quem quiser confidencialidade, assinatura ou encapsulamento seguro pode aplicar uma camada externa como PGP.

## Relação com o restante da documentação

- A arquitetura atual está em [architecture.md](architecture.md)
- O formato de mensagem está em [message-format.md](message-format.md)
- O protocolo v0.1 está em [protocol-v0.1.md](protocol-v0.1.md)
