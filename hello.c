#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_BACKLOG 10
#define PORT "8080"

int init_server()
{

    struct addrinfo hints;
    struct addrinfo *bind_addr;

    memset(&hints, 0, sizeof(hints)); //zero out!

    hints.ai_family = AF_INET;       // we need ipv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // PASSIVE mode

    getaddrinfo(0, PORT, &hints, &bind_addr); //get local address from system to use in bind().

    //init socket
    int socketfd = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);

    if (bind(socketfd, bind_addr->ai_addr, bind_addr->ai_addrlen) == -1)
    {
        return -1;
    }

    return socketfd;
}

int accept_and_send(int socketfd)
{

    struct sockaddr_storage client_addr; //to store client addr
    socklen_t client_addr_len;           //length of client addr;

    int client = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client == -1)
    {
        printf("cannot accept connection from client!\n");
        return -1;
    }

    char addr_buffer[100]; //empty buf. to store ip.

    getnameinfo((struct sockaddr *)&client_addr, client_addr_len, addr_buffer, sizeof(addr_buffer), 0, 0, NI_NUMERICHOST);

    printf("client IP: %s\n", addr_buffer);

    // HANDLE / READ REQ
    char request[1024];

    int bytes_received = recv(client, request, 1024, 0);

    //send response
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Connection: close\r\n"
                           "Content-Type: text/plain\r\n\r\n"
                           "Hello World from hello.c";

    int bytes_sent = send(client, response, strlen(response), 0);

    close(client);
}

int main()
{

    int socketfd = init_server();

    if (socketfd < 0)
    {
        printf("cannot init_server()");
        return 1;
    }

    printf("socketfd = %d\n", socketfd);

    //start listining
    if (listen(socketfd, MAX_BACKLOG) >= 0)
    {
        printf("✅ server listining on 127.0.0.1:%s\n", PORT);
    }
    else
    {
        printf("❌ cannot listen()! %d \n", listen(socketfd, MAX_BACKLOG));
    }

    while (true)
    {
        accept_and_send(socketfd);
    }

    return 0;
}