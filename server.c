#include "protocol.h"

// This is the server side of the application. It listens for incoming connections and processes requests from clients.
// falta implementar o uso de threads para lidar com multiplas conexoes e evitar bloqueio do servidor

#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define MAX_CONNECTIONS 10
#define NUM_COMANDOS 5

bool acceptRequest ();
bool processRequest (int socket, char *buffer);
bool sendResponse (int socket, char *buffer);
bool openConnection (int port, int value);
bool closeConnection ();

bool BUY (Request* req, int socket);
bool LIST (Request* req, int socket);
bool CANCEL (Request* req, int socket);
bool STATUS (Request* req, int socket);
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
            perror("read50");
            exit(EXIT_FAILURE);
        }
        if (processRequest(new_socket,buffer)){
            printf("OK 1\n");
            memset (buffer, 0, sizeof(buffer));
            continue;
        }else{
            printf("Unsucessful request\n");
            Request req;
            interpretar_cmd(buffer, &req);
            if (e_comando(&req, CMD_EXIT))
            {
                break;
            }
            memset (buffer, 0, sizeof(buffer));
            continue;
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
        if (e_comando(&req, comandos[i]) == 0) {
            // Processar o comando específico
            // Exemplo: se for "BUY", verificar o estoque e atualizar
            // Se for "LIST", retornar a lista de itens disponíveis
            // etc.

            if (e_comando(&req, CMD_BUY) == 0) {
                // Verificar estoque e atualizar
                // Exemplo: procurar o item no estoque e diminuir a quantidade
                if (BUY(&req, socket)){
                    printf("Compra realizada: %s, quantidade: %d\n", req.item, req.quantidade);
                    char response[200];
                    snprintf(response, sizeof(response), "Compra realizada do item %s em %d quantidade\n", req.item, req.quantidade);
                    send(socket, response, strlen(response), 0);
                } else {
                    printf("Compra falhou: %s, quantidade: %d\n", req.item, req.quantidade);
                    char response[200];
                    snprintf(response, sizeof(response), "Compra falhou do item %s em %d quantidade\n", req.item, req.quantidade);
                    send(socket, response, strlen(response), 0);
                }
                return true;

            } else if (e_comando(&req, CMD_LIST) == 0) {
                // Retornar a lista de itens disponíveis
                printf("chegou aqui LIST\n");
                LIST(&req, socket);
                return true;
            } else if (e_comando(&req, CMD_CANCEL) == 0) {
                // Cancelar um pedido
                CANCEL(&req, socket);
                return true;
            } else if (e_comando(&req, CMD_STATUS) == 0) {
                // Retornar o status de um pedido
                STATUS(&req, socket);
                return true;
            } else if (e_comando(&req, CMD_EXIT) == 0) {
                // Fechar a conexão com o cliente
                //close(socket);
                return true; // Indicar que a conexão foi fechada
            }
        }
    }

    return false;

}


bool BUY (Request* req, int socket){
   
    char response1[25] = "COMPROU EBA!\n";
    char response2[25] = "NAO COMPROU!\n";
    for (int i = 0; i < 10; i++) {
        if (strcmp(frutasEstoque[i].nome, req->item) == 0) {
            if (frutasEstoque[i].qtd_stock >= req->quantidade) {
                frutasEstoque[i].qtd_stock -= req->quantidade;
                //send(socket, response1, strlen(response1), 0);
                return true; // Compra bem-sucedida
            } else {
                //send(socket, response2, strlen(response2), 0);
                return false; // Estoque insuficiente
            }
        }
    }
    return false; // Item não encontrado
}


bool LIST (Request* req, int socket){
    // Retornar a lista de itens disponíveis
    // Exemplo: construir uma string com os itens e suas quantidades
    char response[TAMANHO_BUFFER] = {0};
    for (int i = 0; i < 10; i++) {
        char item_info[200];
        snprintf(item_info, sizeof(item_info), "%s: %d\n", frutasEstoque[i].nome, frutasEstoque[i].qtd_stock);
        strncat(response, item_info, sizeof(response) - strlen(response) - 1);
    }
    // Enviar a resposta para o cliente
    send(socket, response, strlen(response), 0);
    return true;
}
    

/*
bool LIST (Request* req, int socket){
    char response[TAMANHO_BUFFER];
    int offset = 0;

    for (int i = 0; i < 10; i++) {
        int written = snprintf(response + offset,
                               sizeof(response) - offset,
                               "%s: %d\n",
                               frutasEstoque[i].nome,
                               frutasEstoque[i].qtd_stock);

        if (written < 0 || written >= sizeof(response) - offset) {
            // buffer full or error
            break;
        }

        offset += written;
    }

    send(socket, response, offset, 0);
    return true;

}
*/
bool CANCEL (Request* req, int socket){
    char response[50] = "Cancelado com sucesso\n";
    char response1[50] = "FALHOU COM O CANCELAMENTO";

    if (e_comando(req, CMD_CANCEL) == 0) {
        // Lógica para cancelar um pedido
        // Exemplo: procurar o item no estoque e aumentar a quantidade
        for (int i = 0; i < 10; i++) {
            if (strcmp(frutasEstoque[i].nome, req->item) == 0) {
                if ((frutasEstoque[i].qtd_stock + req->quantidade) <= MAX_QTD){
                    frutasEstoque[i].qtd_stock += req->quantidade;
                    send(socket, response, sizeof(response),0);
                    return true; // Cancelamento bem-sucedido
                }

            }
        }
    }
    send(socket, response1, sizeof(response1), 0);
    return false;

}
bool STATUS (Request* req, int socket){
    for (int i = 0; i < 10; i++) {
        if (strcmp(frutasEstoque[i].nome, req->item) == 0) {
            char response[200];
            snprintf(response, sizeof(response), "Status do item %s: %d em estoque\n", frutasEstoque[i].nome, frutasEstoque[i].qtd_stock);
            send(socket, response, strlen(response), 0);
            return true; // Status enviado com sucesso
        }
    }
}

