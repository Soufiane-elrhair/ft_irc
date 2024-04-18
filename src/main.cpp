
#include "network/Server.hpp"

int main(int argc, char **argv)
{
    if (argc != 3)
        throw std::runtime_error("Usage: ./ircserv <port> <password>");
    
    Server  server(argv[1], argv[2]);

    // try
    // {
    //     return 0;
    // }
    // catch (const std::exception& e)
    // {
    //     std::cerr << e.what() << std::endl;
    //     return 1;
    // }
}
