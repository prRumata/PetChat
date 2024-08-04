#include "Client.h"
#include <cstring>
#include <cstdlib>

int main(int argc, char *argv[])
{
    char ip[16] = "127.0.0.1";
    if (argc > 2)
    {
        strncpy(ip, argv[1], 16);
        ip[15] = '\0';
    }

    int port = 12345;
    if (argc > 2)
    {
        port = atoi(argv[2]);
    }

    Client client(ip, port);
    client.start();
    return 0;
}
