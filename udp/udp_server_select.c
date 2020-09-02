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

int main()
{

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    struct addrinfo *bind_addr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(0, "8080", &hints, &bind_addr);

    //bind it
    int socket_fd = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
    printf("socket_fd: %d\n", socket_fd);

    if (socket_fd < 0)
    {
        return 1;
    }

    if (bind(socket_fd, bind_addr->ai_addr, bind_addr->ai_addrlen) < 0)
    {
        return 1;
    }
    else
    {
        printf("UDP server listing on 127.0.0.1:8080\n");
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_fd, &master);

    int max_sockets = socket_fd;

    while (true)
    {

        fd_set copy;
        FD_ZERO(&copy);
        FD_SET(socket_fd, &master);
        copy = master;

        select(max_sockets + 1, &copy, 0, 0, 0);

        for (int i = 0; i <= max_sockets; i++)
        {
            if (FD_ISSET(i, &copy))
            {
                if (i == socket_fd)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);

                    char read[1024];
                    int bytes_received = recvfrom(socket_fd, read, 1024, 0, (struct sockaddr *)&client_address, &client_len);

                    printf("%s\n", read);
                }
            }
        }
    }

    close(socket_fd);
}