
#include "../headers/server.hpp"

std::string Server::parseMessage(std::string message, int fd)
{
    Request request(parseRequest(message));
    
    if (request.status == 1)
        return ("Invalid message format\n");
    if (request.method.find("CAP") != std::string::npos)
        return (serverCap(request, fd));
    if (request.method.find("connect") != std::string::npos)
        return (connect(request, fd));
    else if (request.method.find("nick") != std::string::npos || request.method.find("NICK") != std::string::npos)
        return (nick(request, fd));
    else if (request.method.find("user") != std::string::npos)
        return (user(request, fd));
    else if (request.method.find("join") != std::string::npos || request.method.find("JOIN") != std::string::npos)
        return (join(request, fd));
    else if(request.method.find("mode") != std::string::npos || request.method.find("MODE") != std::string::npos)
        return (mode(request, fd));
    else if (request.method.find("topic") != std::string::npos || request.method.find("TOPIC") != std::string::npos)
        return (topic(request, fd));
    else if (request.method.find("privmsg") != std::string::npos || request.method.find("PRIVMSG") != std::string::npos)
        return (privmsg(request, fd));
    else if (request.method.find("kick") != std::string::npos || request.method.find("KICK") != std::string::npos)
        return (kick(request, fd));
    else if (request.method.find("invite") != std::string::npos || request.method.find("INVITE") != std::string::npos)
        return (invite(request, fd));
    else if (request.method.find("quit") != std::string::npos)
        return (quit(request, fd));
    else if (this->client[fd] != NULL && this->client[fd]->getauthentification() == 1 && request.method[0] != '/' && this->client[fd]->getchannel() != "")
    {
        std::string response = "";
        for (std::map<std::string, Channel*>::iterator it = this->_channels.begin(); it != this->_channels.end(); ++it)
        {
            if (it->second->isMember(this->client[fd]))
            {
                response += it->second->getName() + " " + this->client[fd]->getnickname() + " :" + message;
            }
        }
        return (send_to_members(this->client[fd]->getchannel(), response, fd), "");
        // return (send_to_all_members(this->client[fd]->getchannel(), response), "");
    }
    return ("MZL\n");
}

void Server::send_to_members(std::string channel, std::string message, int fd)
{
;
    this->_channels[channel]->sendToMembers(message, fd);
}
void Server::send_to_all_members(std::string channel, std::string message)
{

    this->_channels[channel]->sendToAllMembers(message);
}

Request Server::parseRequest(std::string message)
{
    Request request;
    size_t x = 0;
    size_t y = 0;

    if (message[0] == ' ' || message[0] == '\n' || message[0] == '\0')
    {
        request.status = 1;
        return (request);
    }

    while (message[x] != '\0')
    {
        if (message[x] == ' ')
        {
            request.arguments.push_back(message.substr(y, x - y));
            y = x + 1;
        }
        x++;
    }

    request.arguments.push_back(message.substr(y, x - y));
    request.method = request.arguments[0];
    if (request.arguments[request.arguments.size() - 1][request.arguments[request.arguments.size() - 1].size() - 1] == '\n')
        request.arguments[request.arguments.size() - 1].erase(request.arguments[request.arguments.size() - 1].size() - 1, 1);
    return (request);
}
