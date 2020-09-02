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

int main(){

    struct addrinfo hints;
    memset(&hints, 0 , sizeof(hints));

    struct addrinfo *bind_addr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, "8080", &hints, &bind_addr);

    //bind it
    int socket_fd = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
    printf("socket_fd: %d\n", socket_fd);

    if (socket_fd < 0){
        return 1;
    }

    if ( bind(socket_fd, bind_addr->ai_addr, bind_addr->ai_addrlen) < 0 ){
        return 1;
    }
    else{
        printf("UDP server listing on 127.0.0.1:8080\n");
    }

    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);

    char read[1024];
    int bytes_received = recvfrom(socket_fd, read, 1024, 0, (struct sockaddr*)&client_address, &client_len);

    printf("%s\n", read);

    close(socket_fd);

}