#include "protocol.h"

#include <sys/socket.h>
#include <netinet/in.h>
/*arpa/inet.h, sys/socket.h, netinet/in.h,
pthread.h, semaphore.h*/
#include <arpa/inet.h>
#include <pthread.h>

bool acceptRequest ();
bool processRequest ();



int main(){
    int skt = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;

    bind(skt, &addr, sizeof(addr));
}

