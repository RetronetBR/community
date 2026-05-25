# Formato de mensagem

## Objetivo

O formato de mensagem da RetroNet v0.1 foi pensado para:

- ser simples;
- ser editável por humanos;
- exigir o mínimo possível do usuário;
- funcionar em clientes antigos e limitados;
- permitir que o cliente ou daemon complete os metadados;
- funcionar bem em redes lentas;
- ser fácil de converter para spool, federação, QWK ou outros formatos;
- evoluir sem quebrar clientes antigos.

Este documento descreve o formato lógico e textual de mensagem usado pela
RetroNet. Ele não é uma especificação binária definitiva.

## Regra de ouro

> Regra de ouro: clientes simples escrevem mensagens simples. Clientes, daemons e nós completam o envelope.

```text
O formato mínimo é a interface pública do usuário.
O envelope completo é responsabilidade da infraestrutura RetroNet.
```

## Formato mínimo para mensagem privada

Para uma mensagem privada simples, o usuário ou cliente limitado deve precisar
apenas de um destino, um assunto e do corpo da mensagem.

```text
to: user@node
subject: assunto
---
texto da mensagem
```

Exemplo:

```text
to: joao@node-b
subject: Teste RetroNet
---
Olá João,

teste de mensagem pela RetroNet.
```

Regras do formato mínimo:

- `to` indica o destino lógico.
- `subject` indica o assunto da mensagem.
- `user@node` representa usuário e nó de destino.
- O separador `---` encerra os cabeçalhos digitados pelo usuário.
- Tudo depois do separador é corpo da mensagem.
- `to:` em minúsculo deve ser aceito.
- `subject:` em minúsculo deve ser aceito.
- O `rnmsg` deve normalizar `to:` para `To:` no envelope interno.
- O `rnmsg` deve normalizar `subject:` para `Subject:` no envelope interno.
- O usuário não deve preencher os campos operacionais.

## Campos que o usuário não deve precisar preencher

O usuário comum não deve precisar informar:

- `From`
- `MsgID`
- `Date`
- `SourceNode`
- `DestUser`
- `DestNode`
- `Type`
- `Status`
- `Encoding`
- `Format`
- `Flags`
- `TTL`
- `Priority`
- `Route`
- `HopCount`
- `TrustState`

Esses campos são operacionais e devem ser preenchidos por:

- `rnmsg`;
- cliente local;
- daemon local;
- nó RetroNet;
- mecanismo de federação.

## Normalização pelo rnmsg

O `rnmsg` deve:

- ler o rascunho do usuário;
- aceitar `to:` em minúsculo;
- aceitar `subject:` em minúsculo;
- normalizar `to:` para `To:`;
- normalizar `subject:` para `Subject:`;
- descobrir o usuário local pelo `client.conf`, sessão autenticada ou daemon local;
- descobrir o nó local;
- separar usuário e nó do destino;
- gerar `MsgID`;
- gerar `Date`;
- definir `SourceNode`;
- definir `DestUser`;
- definir `DestNode`;
- definir `Type: private`, quando houver `To`;
- definir `Status: queued`;
- definir `Encoding` conforme plataforma ou configuração;
- definir `Format: plain`;
- preparar o envelope para spool, envio ou federação.

Entrada do usuário:

```text
to: joao@node-b
subject: Teste
---
Olá
```

Envelope interno gerado:

```text
From: maria@node-a
To: joao@node-b
Subject: Teste
MsgID: node-a-20260523-000001
Date: 2026-05-23T18:20:00-03:00
SourceNode: node-a
DestUser: joao
DestNode: node-b
Type: private
Status: queued
Encoding: cp437
Format: plain
Client: rnmsg
---
Olá
```

O segundo formato não precisa ser escrito manualmente. Ele é usado internamente
para spool, auditoria, roteamento e federação.

## Três níveis de mensagem

### 1. Rascunho do usuário

Formato mínimo, humano e editável.

```text
to: joao@node-b
subject: Teste
---
Olá
```

Esse é o formato preferencial para clientes limitados.

### 2. Envelope local

Gerado pelo `rnmsg` antes de colocar a mensagem no spool local.

```text
From: maria@node-a
To: joao@node-b
Subject: Teste
MsgID: node-a-20260523-000001
Date: 2026-05-23T18:20:00-03:00
SourceNode: node-a
DestUser: joao
DestNode: node-b
Type: private
Status: queued
Encoding: cp437
Format: plain
Client: rnmsg
---
Olá
```

### 3. Envelope de transporte/federação

Reservado para daemon ou nó, podendo conter informações adicionais.

```text
From: maria@node-a
To: joao@node-b
Subject: Teste
MsgID: node-a-20260523-000001
Date: 2026-05-23T18:20:00-03:00
SourceNode: node-a
DestUser: joao
DestNode: node-b
Type: private
Status: queued
Encoding: cp437
Format: plain
Client: rnmsg
Route: node-a,node-b
HopCount: 1
TrustState: accepted
---
Olá
```

Clientes antigos não precisam conhecer nem interpretar esse terceiro nível.

## Modo interativo

Clientes ainda mais simples podem nem exigir que o usuário escreva `to:`
manualmente.

```text
rnmsg send joao@node-b "Teste"
```

O programa pode receber o assunto por argumento, abrir um editor, ler do teclado
ou receber texto do usuário.

```text
Olá João
```

Internamente, o cliente monta:

```text
To: joao@node-b
Subject: Teste
---
Olá João
```

Depois o `rnmsg`, daemon ou nó completa o envelope.

## Uso com pipe e stdin

Em sistemas Unix-like, scripts e gateways podem enviar mensagens por pipe ou
entrada padrão.

```text
echo "Olá João" | rnmsg send joao@node-b "Teste"
rnmsg send joao@node-b "Teste" < mensagem.txt
```

Isso facilita automação, integração com ferramentas externas, gateways e testes.

## Considerações para clientes DOS

Clientes DOS devem poder trabalhar com o mínimo possível.

Regras recomendadas:

- o usuário deve digitar pouco;
- o parser deve ser tolerante;
- `to:` em minúsculo deve ser aceito;
- `subject:` em minúsculo deve ser aceito;
- `To:` normalizado deve ser usado no envelope interno;
- `Subject:` normalizado deve ser usado no envelope interno;
- cabeçalhos desconhecidos podem ser ignorados;
- o cliente pode trabalhar com CP437;
- o corpo pode ter limite menor;
- o cliente DOS não precisa suportar todos os headers avançados;
- o cliente pode enviar apenas destino, assunto e corpo ao daemon local;
- o daemon local pode completar o envelope;
- clientes antigos não devem quebrar quando novos campos forem adicionados.

## Formato mínimo para boards

Posts em boards também devem seguir a filosofia de simplicidade.

Rascunho do usuário:

```text
board: retro.hardware
subject: Adaptadores seriais
---
Estou testando um ESP32 como modem serial/Wi-Fi.
```

Envelope gerado:

```text
From: maria@node-a
Board: retro.hardware
Subject: Adaptadores seriais
MsgID: node-a-20260523-000002
Date: 2026-05-23T18:30:00-03:00
SourceNode: node-a
Type: board
Status: queued
Encoding: cp437
Format: plain
---
Estou testando um ESP32 como modem serial/Wi-Fi.
```

Regras para boards:

- `board:` define o board de destino.
- `subject:` é recomendado, mas pode ser opcional em clientes muito simples.
- O `rnmsg` ou cliente deve normalizar para `Board:` e `Subject:`.
- O usuário não precisa preencher dados de federação.
- O nó decide se replica o board.
- Clientes antigos podem consumir isso via formato simplificado ou QWK.

## Tabela de campos

| Campo | Obrigatório | Preenchido por | Descrição |
| --- | --- | --- | --- |
| `To` | sim para mensagem privada | usuário, argumento do `rnmsg` ou cliente | destino lógico |
| `Board` | sim para post em board | usuário, argumento do `rnmsg` ou cliente | board de destino |
| `Subject` | recomendado | usuário ou cliente | assunto da mensagem |
| `From` | sim no envelope | `rnmsg`, daemon ou nó | remetente autenticado |
| `MsgID` | sim no envelope | `rnmsg`, daemon ou nó | identificador único |
| `Date` | sim no envelope | `rnmsg`, daemon ou nó | data de criação |
| `SourceNode` | sim no envelope | `rnmsg`, daemon ou nó | nó de origem |
| `DestUser` | sim para mensagem privada | `rnmsg`, daemon ou nó | usuário extraído de `To` |
| `DestNode` | sim para mensagem privada | `rnmsg`, daemon ou nó | nó extraído de `To` |
| `Type` | sim no envelope | `rnmsg`, daemon ou nó | `private`, `board`, `system`, `registry` ou `notify` |
| `Status` | sim no spool | daemon ou nó | `draft`, `queued`, `sent`, `delivered`, `failed` etc. |
| `Encoding` | recomendado | `rnmsg` ou cliente | `ascii`, `cp437`, `iso-8859-1` ou `utf-8` |
| `Format` | recomendado | `rnmsg` ou cliente | `plain` por padrão |
| `Client` | opcional | cliente | nome do cliente que gerou a mensagem |
| `Route` | interno | nó ou federação | rota entre nós |
| `HopCount` | interno | nó ou federação | quantidade de saltos |
| `TrustState` | interno | nó ou federação | estado de confiança entre nós |

Campos preenchidos pelo usuário devem ser poucos. Campos internos podem crescer
com o tempo.

Clientes antigos devem ignorar campos desconhecidos. Novos campos não devem se
tornar obrigatórios para clientes antigos.

## Regras de parsing

Regras simples para v0.1:

- ler cabeçalhos até encontrar uma linha contendo apenas `---`;
- cada cabeçalho usa o formato `Nome: valor`;
- nomes de campos devem ser simples e sem espaços;
- `to`, `board` e `subject` em minúsculo devem ser aceitos no rascunho;
- o envelope interno deve preferir nomes normalizados: `To`, `Board`, `Subject`;
- espaços antes e depois do valor podem ser removidos;
- campos desconhecidos devem ser ignorados ou preservados quando possível;
- campos duplicados devem ser evitados;
- em caso de duplicidade, o parser v0.1 pode usar o primeiro valor e registrar aviso;
- o corpo não deve ser reinterpretado pelo parser;
- clientes não devem executar conteúdo recebido.

## Limites recomendados para v0.1

Limites conservadores para a primeira versão:

- linha de cabeçalho: até 255 caracteres;
- nome do campo: ASCII, sem espaços;
- corpo recomendado para cliente DOS simples: até 32 KiB;
- mensagens maiores podem ser divididas, paginadas ou empacotadas;
- headers desconhecidos devem ser ignorados;
- ordem dos headers não deve ser obrigatória;
- o separador `---` deve aparecer sozinho na linha;
- quebras de linha devem poder ser normalizadas para CRLF ao exportar para DOS/QWK.

Esses limites são recomendações iniciais, não um contrato definitivo.

## Relação com capabilities

Capabilities permitem que o servidor entregue apenas o que o cliente entende.

Exemplos:

- `RN_CAP_NOTIFY`: cliente recebe apenas notificações curtas;
- `RN_CAP_BOARD`: cliente pode receber posts de boards;
- `RN_CAP_QWK`: cliente pode receber pacote QWK;
- `RN_CAP_FILE`: cliente pode receber referências de arquivos;
- `RN_CAP_AUTH`: cliente consegue autenticar;
- `RN_CAP_MOTD`: cliente recebe mensagem do dia.

Clientes antigos não precisam suportar tudo. O servidor pode adaptar a entrega.
Novos recursos não devem exigir novos campos manuais do usuário.

## Relação com QWK

QWK pode ser usado como formato de exportação e importação para clientes
antigos.

O formato RetroNet deve preservar campos suficientes para conversão. Campos como
`Board`, `Subject`, `From`, `Date`, `MsgID`, `ReplyTo` e `ThreadID` ajudam nessa
ponte.

O objetivo não é substituir leitores QWK antigos, mas permitir integração.
Clientes antigos podem receber mensagens já convertidas sem entender o envelope
RetroNet completo.

## Segurança e confiança

Regras básicas:

- o formato de mensagem não deve carregar comandos executáveis;
- clientes não devem executar scripts vindos do corpo da mensagem;
- anexos e binários devem ser tratados fora do corpo principal;
- eventos de sistema devem ser validados pelo nó;
- mensagens de registry devem respeitar o modelo de confiança/federação;
- mensagens suspeitas podem ser colocadas em quarentena;
- clientes limitados devem preferir exibir texto puro.

## Exemplos completos

### Mensagem privada mínima

Digitada pelo usuário:

```text
to: joao@node-b
subject: Teste
---
Olá João
```

Envelope gerado:

```text
From: maria@node-a
To: joao@node-b
Subject: Teste
MsgID: node-a-20260523-000001
Date: 2026-05-23T18:20:00-03:00
SourceNode: node-a
DestUser: joao
DestNode: node-b
Type: private
Status: queued
Encoding: cp437
Format: plain
Client: rnmsg
---
Olá João
```

### Post mínimo em board

Digitado pelo usuário:

```text
board: retro.hardware
subject: Adaptadores seriais
---
Estou testando um ESP32 como modem serial/Wi-Fi.
```

Envelope gerado:

```text
From: maria@node-a
Board: retro.hardware
Subject: Adaptadores seriais
MsgID: node-a-20260523-000002
Date: 2026-05-23T18:30:00-03:00
SourceNode: node-a
Type: board
Status: queued
Encoding: cp437
Format: plain
---
Estou testando um ESP32 como modem serial/Wi-Fi.
```

### Notificação curta

Mensagem para cliente com `RN_CAP_NOTIFY`:

```text
Type: notify
To: maria@node-a
Priority: low
---
Você possui 3 novas mensagens.
```

RNTSR e clientes limitados podem receber apenas notificações curtas, sem baixar
mensagens completas.

## Compatibilidade

O formato completo existe para transporte, spool, auditoria e federação.

O formato mínimo existe para o usuário e clientes limitados.

A evolução do protocolo não deve exigir que clientes antigos preencham novos
campos. Novos campos devem ser opcionais ou preenchidos pelo nó.

Clientes antigos devem continuar funcionando se souberem apenas destino, board,
assunto e corpo.

Compatibilidade é mais importante que sofisticação.

## Relação com outros documentos

- O fluxo de transporte está em [Protocolo v0.1](protocol-v0.1.md).
- A federação de usuários está em [Federação v0.1](federation-v0.1.md).
- As capacidades estão em [Capabilities](capabilities.md).
