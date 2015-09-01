#include "main.h"
#include "LoginServer.h"

int main(int argc, char* argv[])
{
    modou::LoginServer server("127.0.0.1", 2048);
    server.start();
    return 0;
}
