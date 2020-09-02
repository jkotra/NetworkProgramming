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

    if (argc < 2){
        printf("invalid no of args\n");
        return 1;
    }

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

    char *header = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\n\r\n";
    send(socket_fd, header, strlen(header), 0);

    char res[4096];
    int bytes_received = recv(socket_fd, res, 4096, 0);
    printf("bytes_received: %d\n", bytes_received);

    printf("%s\n", res);

    close(socket_fd);
}
