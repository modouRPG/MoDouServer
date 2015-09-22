#include "main.h"
#include "ChatServer.h"

int main(int argc, char* argv[])
{
    modou::ChatServer server("127.0.0.1", 2050);
    server.start();
    return 0;
}
