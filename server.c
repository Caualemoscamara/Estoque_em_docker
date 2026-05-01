#include "protocol.h"

// This is the server side of the application. It listens for incoming connections and processes requests from clients.
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PORT 8080
#define MAX_CONNECTIONS 10
#define NUM_COMANDOS 5

bool acceptRequest ();
bool processRequest (int socket, char *buffer);
bool sendResponse (int socket, char *buffer);
bool openConnection (int port, int value);
bool closeConnection ();

bool BUY (Request* req);
bool LIST (Request* req);
bool CANCEL (Request* req);
bool STATUS (Request* req);
bool EXIT (Request* req);

char *comandos[NUM_COMANDOS] = {CMD_LIST, CMD_BUY, CMD_CANCEL, CMD_STATUS, CMD_EXIT};

int main(){

    // inicializacao do servidor
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[TAMANHO_BUFFER] = {0};

    // Criar socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }


    // Configurar endereço
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Anexar socket ao porta 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Ouvir por conexões
    if (listen(server_fd, MAX_CONNECTIONS) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }


    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
   
    // usar treads para lidar com cada requisisao
    // Processar requisições
    while (true) {
        // Processar requisição
        if (read(new_socket, buffer, TAMANHO_BUFFER) < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        // Enviar resposta
        if (send(new_socket, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}


bool processRequest (int socket, char *buffer){
    // Processar a requisição do cliente e gerar uma resposta
    // Exemplo: interpretar o comando e atualizar o estoque

    Request req;
    interpretar_cmd(buffer, &req);


    for (int i = 0; i < NUM_COMANDOS; i++) {
        if (e_comando(req.comando, comandos[i]) == 0) {
            // Processar o comando específico
            // Exemplo: se for "BUY", verificar o estoque e atualizar
            // Se for "LIST", retornar a lista de itens disponíveis
            // etc.

            if (e_comando(req.comando, CMD_BUY) == 0) {
                // Verificar estoque e atualizar
                // Exemplo: procurar o item no estoque e diminuir a quantidade
                if (BUY(&req)){
                    printf("Compra realizada: %s, quantidade: %d\n", req.item, req.quantidade);
                } else {
                    printf("Compra falhou: %s, quantidade: %d\n", req.item, req.quantidade);
                }
                // Enviar resposta para o cliente
                if (send(socket, "Compra realizada com sucesso\n", 30, 0) < 0) {
                    perror("send");
                    exit(EXIT_FAILURE);
                }

            } else if (e_comando(req.comando, CMD_LIST) == 0) {
                // Retornar a lista de itens disponíveis
                LIST(&req);
            } else if (e_comando(req.comando, CMD_CANCEL) == 0) {
                // Cancelar um pedido
                CANCEL(&req);
            } else if (e_comando(req.comando, CMD_STATUS) == 0) {
                // Retornar o status de um pedido
                STATUS(&req);
            } else if (e_comando(req.comando, CMD_EXIT) == 0) {
                // Fechar a conexão com o cliente
                close(socket);
                return false; // Indicar que a conexão foi fechada
            }
        }
    }

    return true;

}


bool BUY (Request* req){
   
    for (int i = 0; i < 10; i++) {
        if (strcmp(frutasEstoque[i].nome, req->item) == 0) {
            if (frutasEstoque[i].qtd_stock >= req->quantidade) {
                frutasEstoque[i].qtd_stock -= req->quantidade;
                return true; // Compra bem-sucedida
            } else {
                return false; // Estoque insuficiente
            }
        }
    }
    return false; // Item não encontrado
}
bool LIST (Request* req){
    // Retornar a lista de itens disponíveis
    // Exemplo: construir uma string com os itens e suas quantidades
    char response[TAMANHO_BUFFER] = {0};
    for (int i = 0; i < 10; i++) {
        char item_info[50];
        snprintf(item_info, sizeof(item_info), "%s: %d\n", frutasEstoque[i].nome, frutasEstoque[i].qtd_stock);
        strncat(response, item_info, sizeof(response) - strlen(response) - 1);
    }
    // Enviar a resposta para o cliente
    send(socket, response, strlen(response), 0);
    return true;
}
bool CANCEL (Request* req){
    if (e_comando(req->comando, CMD_CANCEL) == 0) {
        // Lógica para cancelar um pedido
        // Exemplo: procurar o item no estoque e aumentar a quantidade
        for (int i = 0; i < 10; i++) {
            if (strcmp(frutasEstoque[i].nome, req->item) == 0) {
                frutasEstoque[i].qtd_stock += req->quantidade;
                return true; // Cancelamento bem-sucedido
            }
        }
    }

}
bool STATUS (Request* req){
    for (int i = 0; i < 10; i++) {
        if (strcmp(frutasEstoque[i].nome, req->item) == 0) {
            char response[50];
            snprintf(response, sizeof(response), "Status do item %s: %d em estoque\n", frutasEstoque[i].nome, frutasEstoque[i].qtd_stock);
            send(socket, response, strlen(response), 0);
            return true; // Status enviado com sucesso
        }
    }
}

