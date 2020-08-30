#include <sys/types.h>
#include <sys/socket.h>

#include<netinet/in.h>

#include<netdb.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

int init_server(){


    struct addrinfo hints;
    struct addrinfo *bind_addr; 

    memset(&hints, 0, sizeof(hints)); //zero out!

    hints.ai_family = AF_INET6; // we need ipv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE; // PASSIVE mode

    getaddrinfo(0, "8080", &hints, &bind_addr); //get local address from system.
    
    //init socket
    int socketfd = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);


    if (bind(socketfd, bind_addr->ai_addr, bind_addr->ai_addrlen) == -1){
        return -1;
    }

    return socketfd;

}

void accept_and_send(int socketfd){

    struct sockaddr_storage client_addr; //to store client addr
    socklen_t client_addr_len; //length of client addr;

    int client = accept(socketfd, (struct sockaddr*)&client_addr, &client_addr_len);

    if (client == -1){
        printf("cannot accept connection from client!");
        return 1;
    }

    char addr_buffer[100]; //empty buf. to store ip.

    getnameinfo((struct sockaddr*)&client_addr, client_addr_len, addr_buffer, sizeof(addr_buffer), 0, 0, NI_NUMERICHOST);

    printf("addr_buf/client IP: %s\n", addr_buffer);

    // HANDLE / READ REQ
    char request[1024];

    int bytes_received = recv(client, request, 1024, 0);

    printf("bytes received: %d\n=================\n", bytes_received);
    printf("received request: %s\n=================\n", request);


    //send response
    const char *response = "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Hello World from hello.c";

    int bytes_sent = send(client, response, strlen(response), 0);

    printf("%s\n=================\n", response);

    close(client);
}




int main() {

    struct addrinfo* bind_addr;
    int socketfd = init_server(bind_addr);

    if (socketfd < 0){
        printf("cannot init_server()");
        return 1;
    }

    printf("socketfd: %d\n", socketfd);

    int option = 0;
    setsockopt(socketfd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&option, sizeof(option));

    //start listining
    listen(socketfd, 10); //max 10 connections backlog

    while (true)
    {
        accept_and_send(socketfd);
    }
    
    return 0;
}