
#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <string>
# include <vector>

class Request
{
    public:
        std::vector<std::string> arguments;
        std::string method;
        int status;

        Request();
        ~Request();
        Request(Request const &src);
        Request &operator=(Request const &rhs);
} ;

#endif