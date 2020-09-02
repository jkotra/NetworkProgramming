#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netinet/in.h>

#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>

int main(int argc, char *argv[])
{

    if (argc < 3){
        printf("invalid no of args.\n");
        return 1;
    }

    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    struct addrinfo *peer;
    hints.ai_socktype = SOCK_DGRAM;

    printf("args: %s %s %s\n", argv[1], argv[2], argv[3]);

    if (getaddrinfo(argv[1], argv[2], &hints, &peer) < 0)
    {

        printf("addrinfo() error!\n");
        return 1;
    }

    //create socket
    int socket_fd = socket(peer->ai_family, peer->ai_socktype, peer->ai_protocol);
    
    char msg[1024];
    strncpy(msg, argv[3], 1024);
    int bytes_sent = sendto(socket_fd, msg, strlen(msg), 0, peer->ai_addr, peer->ai_addrlen);

    printf("Sent %s (%d bytes)\n", msg, bytes_sent);

    close(socket_fd);


}