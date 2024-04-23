
#include "../headers/header.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Check your args!" << std::endl;
        exit(1);
    }
    Server server = Server(av);

    server.run();
    return (0);
}