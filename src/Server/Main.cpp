#include "Server.h"
#include <cstdlib>

int main(int argc, char *argv[])
{
    int port = 12345;
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    Server server(port);
    server.Start();
    return 0;
}
