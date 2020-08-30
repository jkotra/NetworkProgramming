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

    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));

    struct addrinfo *peer;
    hints.ai_socktype = SOCK_STREAM;

    printf("args: %s %s\n", argv[1], argv[2]);

    if (getaddrinfo(argv[1], argv[2], &hints, &peer) < 0)
    {

        printf("addrinfo() error!\n");
        return 1;
    }

    char peer_addr[100];
    char peer_protocol[100];
    getnameinfo(peer->ai_addr, peer->ai_addrlen, peer_addr, sizeof(peer_addr), peer_protocol, sizeof(peer_protocol), NI_NUMERICHOST);

    printf("ip: %s | protocol: %s\n", peer_addr, peer_protocol);

    //family socket_type protocol
    int socket_fd = socket(peer->ai_family, peer->ai_socktype, peer->ai_protocol);

    if (socket_fd < 0)
    {
        printf("socket error.\n");
        return 1;
    }

    if (connect(socket_fd, peer->ai_addr, peer->ai_addrlen) < 0)
    {
        printf("connect error.\n");
        return 1;
    }

    printf("connected!\n");


    fd_set all_sockets;
    FD_ZERO(&all_sockets);
    FD_SET(socket_fd, &all_sockets);
    FD_SET(0, &all_sockets); //stdin
    

    while (true)
    {

        fd_set copy;
        FD_ZERO(&copy);
        copy = all_sockets;

        select(socket_fd+1, &copy, 0,0,0 );
        
        if ( FD_ISSET(0, &copy) ){
        char read[4096];

        if ( fgets(read, 4096, stdin) ){
            printf("sending: %s\n", read);
            int b_out = send(socket_fd, read, strlen(read), 0);
            printf("sent %d bytes\n", b_out);
        }

        }

        if ( FD_ISSET(socket_fd, &copy) ){
            char resp[4096];
            int bytes_received = recv(socket_fd, resp, 4096, 0);
            printf("%s\n", resp);

            break;

        }

    }

}
