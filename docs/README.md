# RetroNet Community

RetroNet é uma rede store-and-forward inspirada em UUCP, QWK, FidoNet e BBS clássicas.

Voltar ao [README principal](../README.md).

O foco do projeto é comunidade, portabilidade e simplicidade operacional:

- rede descentralizada
- sem ponto único de falha
- compatível com hardware limitado
- preparada para DOS, 8088, Z80 e sistemas antigos
- mensagens offline
- transporte agnóstico

O estado atual do MVP cobre:

- criação de rascunhos e mensagens
- envio e recebimento via spool local
- roteamento por alias e peer
- replicação básica entre nós
- registro federado básico de usuários
- arquivos texto como formato principal de armazenamento

## Documentos

- [Core Documentation](core/README.md)
- [RetroNet SDK](sdk/README.md)
- [Research](research/README.md)

## Árvore resumida

```text
retronet-poc/
  src/
  demo/
  docs/
  runtime/
  Makefile
  README.md
```

## Estado atual

O projeto já possui os executáveis `rnd`, `rnsync`, `rnpeer`, `rnmsg` e `rndctl`.

O fluxo de mensagem segue a lógica store-and-forward descrita em
[Core Documentation](core/README.md).

O registro de usuário e a fonte de verdade de identidade são descritos
em [Core Documentation](core/README.md).
