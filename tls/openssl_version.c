#include <openssl/ssl.h>

int main(int argc, char *argv[]) {

    printf("OpenSSL version: %s\n", OpenSSL_version(SSLEAY_VERSION));

    return 0;
}
