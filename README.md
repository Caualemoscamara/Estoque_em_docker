# 🧾 Sistema de Estoque Distribuído com Sockets TCP (Docker)

## 📌 Descrição

Este projeto implementa um sistema cliente-servidor utilizando **sockets TCP em C**, com suporte a múltiplos clientes simultâneos através de **threads** e controle de concorrência com **mutex**.

O sistema permite operações de gerenciamento de estoque como:

* `LIST` – listar itens disponíveis
* `BUY <item> <qtd>` – comprar item
* `CANCEL <item> <qtd>` – cancelar compra
* `STATUS <item>` – consultar estoque
* `EXIT` – encerrar conexão

Todo o sistema foi **containerizado com Docker**, permitindo execução sem necessidade de compilação manual no host.

---

## 🐳 Estrutura Docker

O projeto utiliza:

* ✅ **Uma única imagem Docker** (contendo servidor e cliente)
* ✅ **Dois contêineres**:

  * `server` → executa o servidor TCP
  * `client` → executa o cliente
* ✅ Comunicação via **rede interna Docker**
* ✅ Cliente conecta usando o hostname: `server`

---

## 🚀 Como executar

### 1. Build e execução

```bash
docker compose up --build
```

---

### 2. Acessar o cliente interativo

Em outro terminal:

```bash
sudo docker exec -it client ./clientDocker server 8080
```

---

## 💻 Comandos disponíveis

Dentro do cliente, utilize:

```text
LIST
BUY laranja 2
STATUS laranja
CANCEL laranja 1
EXIT
```

---

## 👥 Múltiplos clientes

O servidor suporta múltiplos clientes simultâneos.

### Para abrir múltiplos clientes:

```bash
sudo docker exec -it client ./clientDocker server 8080
```

Execute esse comando em **vários terminais diferentes**.

👉 Cada execução representa um novo cliente conectado ao servidor.

---

## 🔒 Concorrência

O sistema utiliza:

* `pthread` para múltiplas conexões simultâneas
* `mutex` para proteger o acesso ao estoque compartilhado

Isso garante que:

```text
✔ Não há corrupção de dados
✔ Operações concorrentes são seguras
```

---

## 🌐 Comunicação

* Protocolo: **TCP (SOCK_STREAM)**
* Porta padrão: **8080**
* Host no Docker: **server**

---

## 🛠️ Compilação (dentro do container)

A compilação é feita automaticamente via:

```bash
make
```

definido no `Dockerfile`.

---

## 📦 Docker Hub

(Preencher com sua imagem)

```bash
docker pull SEU_USUARIO/estoque
docker compose up
```

---

## 📁 Estrutura do projeto

```text
.
├── server.c
├── client.c
├── clientDocker.c
├── clientNGROK.c
├── protocol.h
├── Makefile
├── Dockerfile
├── docker-compose.yml
└── README.md
```

---

## ✅ Requisitos atendidos

✔ Comunicação via sockets TCP
✔ Suporte a múltiplos clientes
✔ Controle de concorrência com mutex
✔ Sistema containerizado com Docker
✔ Execução via docker-compose
✔ Cliente conectado via hostname interno

---

## 🧠 Observações finais

* O cliente utilizado no Docker é o `clientDocker`
* O hostname `server` é resolvido automaticamente pela rede Docker
* Não é necessário configurar IP manualmente

---

## 👨‍💻 Autor(es)

Caua Lemos, Kevin Caley

---

## 📌 Exemplo de execução

```text
client  | Connected to server:8080
> LIST
apple: 10
banana: 5

> BUY apple 2
Compra realizada

> STATUS apple
apple: 8
```

---

## 🏁 Conclusão

Este projeto demonstra a implementação de um sistema distribuído simples com:

* Comunicação em rede
* Concorrência
* Containerização

seguindo boas práticas de sistemas operacionais e redes.
