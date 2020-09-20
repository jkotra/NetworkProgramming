#include <openssl/x509.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>

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

    // ssl initialization.
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    //init context
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx)
    {
        printf("cannot create SSL context!\n");
        return 1;
    }

    /* code taken from tcp_client_classic */
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

    /* END */

    SSL *ssl = SSL_new(ctx);
    if (!ctx)
    {
        fprintf(stderr, "SSL_new() failed.\n");
        return 1;
    }

    if (!SSL_set_tlsext_host_name(ssl, argv[1]))
    {
        fprintf(stderr, "SSL_set_tlsext_host_name() failed.\n");

        return 1;
    }

    SSL_set_fd(ssl, socket_fd);
    if (SSL_connect(ssl) == -1)
    {
        fprintf(stderr, "SSL_connect() failed.\n");
        return 1;
    }

    printf("SSL/TLS using %s\n", SSL_get_cipher(ssl));

    char buffer[2048];

    sprintf(buffer, "GET / HTTP/1.1\r\n");
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", argv[1], argv[2]);
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    sprintf(buffer + strlen(buffer), "User-Agent: https_simple\r\n");
    sprintf(buffer + strlen(buffer), "\r\n");

    SSL_write(ssl, buffer, strlen(buffer));
    printf("Sent Headers:\n%s", buffer);

    int bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytes_received < 1)
    {
        printf("\nConnection closed by peer.\n");
    }

    printf("Received (%d bytes): '%.*s'\n", bytes_received, bytes_received, buffer);
    

    // shutdown ssl connection and free() ctx. 
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    
    // close socket
    close(socket_fd);

    return 0;
}
