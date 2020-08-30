#include <stdio.h>
#include <time.h>
#include <string.h>

int main() {

    time_t now;

    time(&now);

    printf("%s\n", ctime(&now));

    return 0;
}