#include <stdio.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char *argv[])
{

    printf("resolving host: %s\n", argv[1]);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_flags = AI_ALL;

    struct addrinfo *peer;

    if (getaddrinfo(argv[1], 0, &hints, &peer) != 0)
    {
        printf("getaddrinfo error!\n");
        return 1;
    }

    struct addrinfo *address = peer;

    do
    {
        char addr_buf[100];

        getnameinfo(address->ai_addr, address->ai_addrlen, addr_buf, sizeof(addr_buf),
                    0, 0, NI_NUMERICHOST);

        printf("%s\n", addr_buf);
    } while ((address = address->ai_next));

    return 0;
}