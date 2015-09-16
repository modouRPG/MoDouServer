#include "main.h"
#include <iostream>
#include "MapServer.h"
#include "Session.h"

int main(int argc, char *argv[])
{
  modou::MapServer server("127.0.0.1", 2049);
  server.start();
  std::cout << "hello world" << std::endl;
  return 0;
}
