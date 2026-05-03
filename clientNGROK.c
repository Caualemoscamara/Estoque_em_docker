#include "protocol.h"
#include <netdb.h>


int main(){
    struct addrinfo hints, *res;

    // Configure address resolution
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;

    // 🔴 USE NGROK VALUES HERE
    int status = getaddrinfo("0.tcp.sa.ngrok.io", "14550", &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // Create socket
    int usr_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(usr_fd < 0){
        perror("socket");
        return 1;
    }

    // Connect
    if(connect(usr_fd, res->ai_addr, res->ai_addrlen) < 0){
        perror("connect");
        return 1;
    }

    printf("Connected!\n");

    //Coração do usuário
    char buffer[TAMANHO_BUFFER];
    while(true){
        memset(buffer, 0, sizeof(buffer));

        printf("> ");
        if(fgets(buffer, TAMANHO_BUFFER, stdin) == NULL)
            break;

        printf("Conteúdo pré send: %s\n", buffer);

        Request req;
        interpretar_cmd(buffer, &req);

        printf("Request: %s %s %d\n", req.comando, req.item, req.quantidade);

        // EXIT
        if (e_comando(&req, CMD_EXIT) == 0){
            send(usr_fd, buffer, strlen(buffer), 0);
            printf("Encerrando Cliente %d\n", usr_fd);
            break;
        }

        // Send
        if (send(usr_fd, buffer, strlen(buffer), 0) < 0){
            perror("send");
            break;
        }

        // Receive
        int bytes = recv(usr_fd, buffer, TAMANHO_BUFFER-1, 0);
        if(bytes < 0){
            perror("recv");
            break;
        }
        if (bytes == 0) {
            printf("Servidor fechou conexão\n");
            break;
        }

        buffer[bytes] = '\0';
        printf("%s", buffer);
    }

    close(usr_fd);
    freeaddrinfo(res);

    printf("Sucesso!\n");
    return 0;
}

