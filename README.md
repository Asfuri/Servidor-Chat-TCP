# Servidor de Chat Multiusuário TCP - Etapa 2

## 📖 Sobre o Projeto

Este projeto implementa um **Servidor de Chat Multiusuário TCP** desenvolvido em C++ como trabalho final da disciplina de Programação Concorrente. O sistema demonstra o domínio prático de conceitos fundamentais de programação concorrente, incluindo threads, exclusão mútua com mutexes, variáveis de condição e sockets TCP.

### 🎯 Roadmap do Projeto

1. **Etapa 1** ✅ - Arquitetura + biblioteca libtslog thread-safe
2. **Etapa 2** ✅ - Protótipo CLI com comunicação TCP básica
3. **Etapa 3** 🔜 - Sistema completo com monitores e broadcasting otimizado

## ✨ O Que Há de Novo na Etapa 2

Nesta etapa, o projeto evoluiu de um simples teste de logging para um protótipo de cliente/servidor TCP funcional. As principais implementações foram:

- **Servidor TCP Concorrente (`tcp_server`)**: Aceita múltiplas conexões de clientes simultaneamente, criando uma thread dedicada para cada um.
- **Cliente CLI (`tcp_client`)**: Uma interface de linha de comando que permite aos usuários conectar ao servidor, enviar e receber mensagens em tempo real.
- **Broadcasting de Mensagens**: O servidor retransmite as mensagens recebidas para todos os outros clientes conectados, criando uma sala de chat.
- **Sistema de Logs Organizado**: Os logs agora são salvos em um diretório dedicado `logs/`, com arquivos separados para o servidor, cliente e testes.
- **Makefile Avançado**: Novos alvos para compilar, executar e testar o sistema de forma automatizada e organizada.


## 📁 Estrutura do Projeto

A estrutura foi reorganizada para acomodar os novos componentes:

```
Servidor-Chat-TCP/
├── 📂 build/
│   ├── 📁 obj/
│   ├── 📁 logs/
│   └── Makefile
├── 📂 lib/
│   ├── libtslog.h
│   └── logEntry.h
├── 📂 src/
│   ├── libtslog.cpp
│   ├── tcp_server.cpp
│   ├── tcp_client.cpp
│   └── test_libtslog.cpp
├── 📂 img/
│   ├── diagrama-classes.png
│   ├── diagrama-sequencia.png
│   └── fluxograma-logger.png
└── 📄 README.md
└── 📄 .gitignore
```


## 🚀 Como Compilar e Executar

### 📋 Pré-requisitos

- **Compilador**: g++ com suporte a C++17 ou superior
- **Sistema**: Linux/Unix
- **Dependências**: `pthread`


### 🔨 Compilação

Navegue até o diretório `build` e execute `make` para compilar todos os executáveis.

```bash
cd build/
make all
```


### ▶️ Execução Manual

1. **Inicie o Servidor** (em um terminal):

```bash
make run-server
```

O servidor iniciará na porta 8080 e os logs serão salvos em `logs/server.log`.
2. **Inicie o Cliente** (em outro terminal):

```bash
make run-client
```

O cliente se conectará a `localhost:8080`. Os logs do cliente são salvos em `logs/client.log`. Abra múltiplos terminais e execute este comando para simular vários usuários.

### Cliente customizado

```bash
make run-client-custom SERVER_IP=IP SERVER_PORT=PORT
```

### 🧪 Testes Automatizados

O Makefile oferece alvos para testes automatizados que facilitam a verificação da funcionalidade.

- **Teste Básico de Conexão (`test-tcp`)**:

```bash
make test-tcp
```

Este comando inicia o servidor, conecta 3 clientes, envia mensagens, encerra as conexões e exibe um resumo dos logs gerados.
- **Teste de Stress (`stress-test`)**:

```bash
make stress-test
```

Simula 10 clientes conectando e enviando mensagens simultaneamente para verificar a robustez do servidor sob carga. Os logs deste teste são salvos em `logs/stress/`.


## 📊 Gerenciamento de Logs

Com a nova estrutura, os logs estão mais organizados e fáceis de analisar.

- **Ver Resumo dos Logs**:

```bash
make logs-summary
```

Exibe um resumo de todos os arquivos `.log` na pasta `logs/`, incluindo tamanho e número de linhas.
- **Monitorar Logs em Tempo Real**:

```bash
make logs-tail
```

Monitora o arquivo `logs/server.log` em tempo real, útil para depuração.
- **Limpar Logs**:

```bash
make clean-logs
```

Remove todos os arquivos de log da pasta `logs/` sem apagar os executáveis.


## 🚧 Próximas Etapas (Etapa 3)

- Refinar o gerenciamento de clientes com estruturas de dados mais robustas (monitores).
- Implementar tratamento de erros mais detalhado.
- Adicionar comandos no chat.
- Elaborar o relatório final de análise de concorrência.

