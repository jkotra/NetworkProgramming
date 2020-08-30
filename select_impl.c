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

    getaddrinfo(0, PORT, &hints, &bind_addr); //get local address from system.

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
        printf("cannot accept connection from client! Error:%d\n", errno);
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

    //close(client);
    return client;
}

int main()
{

    printf("select() example!\n");

    struct addrinfo *bind_addr;
    int socketfd = init_server(bind_addr);

    if (socketfd < 0)
    {
        printf("cannot init_server() Error: %d\n", errno);
        return 1;
    }

    printf("socketfd = %d\n", socketfd);

    //setup select()
    fd_set master;
    FD_ZERO(&master);
    FD_SET(socketfd, &master);
    int maxsockets = socketfd;

    //start listining
    if (listen(socketfd, MAX_BACKLOG) >= 0)
    {
        printf("server listining on 127.0.0.1:%s\n", PORT);
    }
    else
    {
        printf("cannot listen()! %d\n");
    }

    while (true)
    {


        //make a copy of master fd_set
        fd_set copy;
        copy = master;

        //call select(). this will destruct all non-readable fd's
        select(maxsockets + 1, &copy, NULL, NULL, NULL);

        for (int i = 0; i <= maxsockets; i++)
        {
            if ( FD_ISSET(i, &copy) ){
                printf("-> socket %d is ready to be read!\n", i);

                if (i == socketfd){

                    printf("🌎 new connection on socket: %d\n", i);
                    //new connection
                    int cs = accept_and_send(i);
                    printf("✅ client socket = %d\n", cs);

                    FD_SET(cs, &master);
                    if (cs > maxsockets){ maxsockets = cs; };
                }
                else{
                    printf("🎈 activity on socket %d\n", i);

                    //read and display on to user
                    char buffer[1024];
                    recv(i, buffer, 1024, 0);

                    printf("%d\n", strlen(buffer));
                    
                    //<= 1 means connection closed from client side!
                    if (strlen(buffer) <= 1){
                        printf("❌ closed client socket %d received.length = %d \n", i, strlen(buffer));
                    }

                    //remove from master fd_set
                    FD_CLR(i, &master);

                }

            }
        }
        
    }

    return 0;
}