#include "protocol.h"
#include <netdb.h>


int main(int argc, char *argv[]) {
    // Expect: ./client <host> <port>
    if (argc < 3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        printf("Example (Docker): %s server 8080\n", argv[0]);
        return 1;
    }

    struct addrinfo hints, *res;

    // Configure address resolution
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    // Resolve host and port
    int status = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // Create socket
    int usr_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (usr_fd < 0) {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    // 🔁 Retry loop (important for Docker startup timing)
    int attempts = 0;
    while (connect(usr_fd, res->ai_addr, res->ai_addrlen) < 0) {
        if (attempts >= 5) {
            perror("connect");
            freeaddrinfo(res);
            close(usr_fd);
            return 1;
        }
        printf("Waiting for server...\n");
        sleep(1);
        attempts++;
    }

    printf("Connected to %s:%s\n", argv[1], argv[2]);

    // Main loop
    char buffer[TAMANHO_BUFFER];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        printf("> ");
        if (fgets(buffer, TAMANHO_BUFFER, stdin) == NULL)
            break;

        printf("Conteúdo pré send: %s\n", buffer);

        Request req;
        interpretar_cmd(buffer, &req);

        printf("Request: %s %s %d\n", req.comando, req.item, req.quantidade);

        // EXIT
        if (e_comando(&req, CMD_EXIT) == 0) {
            send(usr_fd, buffer, strlen(buffer), 0);
            printf("Encerrando Cliente %d\n", usr_fd);
            break;
        }

        // Send
        if (send(usr_fd, buffer, strlen(buffer), 0) < 0) {
            perror("send");
            break;
        }

        // Receive
        int bytes = recv(usr_fd, buffer, TAMANHO_BUFFER - 1, 0);
        if (bytes < 0) {
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