#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>

#include <netinet/in.h>

#include <netdb.h>

#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <errno.h>

#define SMTPSERVER "gmail-smtp-in.l.google.com"
#define SMTPPORT "25"


void parse_code(char *server_response, char *resp)
{

    for (int i = 2; i < strlen(server_response); i++)
    {
        if (isdigit(server_response[i]) && isdigit(server_response[i - 1]) && isdigit(server_response[i - 1]))
        {
            resp[0] = server_response[i - 2];
            resp[1] = server_response[i - 1];
            resp[2] = server_response[i];

            break;
        }
    }
}

int wait_for_response(int socket_fd, char *code)
{

    char response[1024];
    char code_resp[4];

    int recv_bytes = recv(socket_fd, response, 1024, 0);
    printf("Server: %s\n", response);

    parse_code(response, code_resp);

    if (strcmp(code_resp, code) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int send_response(int socket_fd, char *request)
{


    printf("Client: %s\n");
    int sent_bytes = send(socket_fd, request, strlen(request), 0);

}

int main(int argc, char *argv[])
{


    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(SMTPSERVER, SMTPPORT, &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed.\n");
        exit(1);
    }


    printf("Creating socket...\n");
    int server;
    server = socket(peer_address->ai_family,
                    peer_address->ai_socktype, peer_address->ai_protocol);

    printf("Connecting...\n");
    if (connect(server,
                peer_address->ai_addr, peer_address->ai_addrlen))
    {
        fprintf(stderr, "connect() failed.\n");
        exit(1);
    }

    freeaddrinfo(peer_address);

    printf("Connected!\n");

    if (!wait_for_response(server, "220")){
        printf("unexpected response\n");
        exit(1);
    }

    send_response(server, "HELO BOOMER\r\n");
    if (!wait_for_response(server, "250")){
        printf("unexpected response\n");
        exit(1);
    }


    send_response(server, "MAIL FROM:<hekko@xyz.com>\r\n");

    if (!wait_for_response(server, "250")){
        printf("unexpected response\n");
        exit(1);
    }


    send_response(server, "RCPT TO:<testing@gmail.com>\r\n");

    if (!wait_for_response(server, "250")){
        printf("unexpected response\n");
        exit(1);
    }

    send_response(server, "DATA\r\n");

    if (!wait_for_response(server, "354")){
        printf("unexpected response\n");
        exit(1);
    }

    send_response(server, "From:<hekko@xyz.com>\r\n");
    send_response(server, "To:<hello@gmail.com>\r\n");
    send_response(server, "Subject:SMTP TEST\r\n");

    time_t timer;
    time(&timer);

    struct tm *timeinfo;
    timeinfo = gmtime(&timer);

    char date[128];
    strftime(date, 128, "%a, %d %b %Y %H:%M:%S +0000", timeinfo);
    sprintf(date, "Date:%s\r\n", date);

    send_response(server, date);
    send_response(server, "\r\n");

    char helloworld_body[128] = "Hello World.\r\n.\r\n";

    send_response(server, helloworld_body);

    if (!wait_for_response(server, "250")){
        printf("unexpected response\n");
        exit(1);
    }
    
    send_response(server, "QUIT\r\n");
    
    if (!wait_for_response(server, "221")){
        printf("unexpected response\n");
        exit(1);
    }

    return 0;
}