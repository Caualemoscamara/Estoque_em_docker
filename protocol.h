
/*implementar comunicação com socket(), bind(), listen(), accept(), connect(), send()
e recv();*/

/*concorrente com threads no servidor*/

/* mutex ou semáforo*/


/*stdio.h, stdlib.h, string.h, unistd.h, arpa/inet.h, sys/socket.h, netinet/in.h,
pthread.h, semaphore.h*/

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>

// diminuir ou aumentar nos esperimentos verificar se muda algo 
#define TAMANHO_BUFFER 2048
#define PORT 8080
#define MAX_QTD 10

#define CMD_LIST   "LIST"
#define CMD_BUY    "BUY"
#define CMD_CANCEL "CANCEL"
#define CMD_STATUS "STATUS"
#define CMD_EXIT   "EXIT"

//Define a estrutura do pedido feito pelo cliente requisiscao
typedef struct{
    char comando[10];
    char item[25];
    int quantidade;
} Request;

// servidor ammount
typedef struct {
    const char nome[25];
    int qtd_stock;
}Item;

Item frutasEstoque[10]={
    {"banana",  MAX_QTD},
    {"maçã",    MAX_QTD},
    {"melancia",MAX_QTD},
    {"pera",    MAX_QTD},
    {"abóbora", MAX_QTD},
    {"morango", MAX_QTD},
    {"abacaxi", MAX_QTD},
    {"abacate", MAX_QTD},
    {"pitaya",  MAX_QTD},
    {"laranja", MAX_QTD},
};

//Insere a linha de comando na Struct Request
static inline void interpretar_cmd(char *input, Request* req){
    memset(req, 0, sizeof(Request));
    sscanf(input, "%s %s %d", req->comando, req->item, &req->quantidade);
}

//Confere se um comando selecionado é válido
static inline int e_comando(Request* req, const char*cmd){
    return strcmp(req->comando, cmd);
}
#endif
