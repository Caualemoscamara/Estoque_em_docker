#include "protocol.h"

int main(){
    //Variáveis
    struct sockaddr_in addr;
    
    //Configurar socket
    int usr_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(usr_fd < 0){
        perror("socket");
        return 1;
    }

    //Definir endereço
    // configurar com o servidor host 
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    //Solicitar conexão com o servidor
    int usr_sl = connect(usr_fd, (struct sockaddr*)&addr, sizeof(addr));
    if(usr_sl < 0){
        perror("connect");
        return 1;
    }

   
    //Coração do usuário
    char buffer[TAMANHO_BUFFER];
    while(true){
        memset(buffer, 0, sizeof(buffer));
        //Lê input
        printf("> ");
        if(fgets(buffer, TAMANHO_BUFFER, stdin) == NULL)
            break;
        //buffer[strlen(buffer)-1] = '\0';
        printf("Conteúdo pré send: %s\n", buffer);
        //Interpretar request
        Request req;
        interpretar_cmd(buffer, &req);

        printf("Request: %s %s %d\n", req.comando, req.item, req.quantidade);
        //Envia comando de saída para o servidor
        if (e_comando(&req, CMD_EXIT) == 0){
            send(usr_fd, buffer, strlen(buffer), 0);
            printf("Encerrando Cliente %d\n", usr_fd);
            break;
        }

        //Envia os outros cmds
        if (send(usr_fd, buffer, strlen(buffer), 0) < 0){
            perror("send");
            break;
        }

        //Recebe retorno do servidor
        int bytes = recv(usr_fd, buffer, TAMANHO_BUFFER-1, 0);
        if(bytes < 0){
            perror("recv");
            break;
        }
        if (bytes == 0) {
            printf("Servidor fechou conexão\n");
            break;
        }
        //Garante que o buffer tenha um \0 no final
        //read não coloca automaticamente
        buffer[bytes] = '\0';

        //Output
        printf("%s", buffer);
    }
    //encerra conexão com o servidor
    close(usr_fd);
    printf("Sucesso!");
    return 0;
}

