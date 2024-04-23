
#include "../headers/request.hpp"

Request::Request(): arguments(), method(), status(0)
{
}

Request::~Request()
{
}

Request::Request(Request const &src)
{
    *this = src;
}

Request &Request::operator=(Request const &rhs)
{
    if (this != &rhs)
    {
        this->arguments = rhs.arguments;
        this->method = rhs.method;
        this->status = rhs.status;
    }
    return (*this);
}