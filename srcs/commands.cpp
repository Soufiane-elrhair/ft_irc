
#include "../headers/server.hpp"

#define BLUE "\e[1;34m"

std::string Server::serverCap(Request request, int fd)
{
    for (size_t i = 0; i < request.arguments.size(); i++)
    {
        if ( i + 1 < request.arguments.size() && request.arguments[i].find("PASS") != std::string::npos)
            {
                int x = 0;
                while (request.arguments[i + 1][x] != '\n' && request.arguments[i + 1][x] != '\0')
                    x++;
                std::string tmp = request.arguments[i + 1].substr(0, x - 1);
                if (tmp == this->password)
                {
                    this->client[fd] = new Client(fd);
                    this->client[fd]->setauthentification(1);
                }
                else
                    return ("Invalid password\n");
            }
        if (i > 0 && request.arguments[i - 1].find("NICK") != std::string::npos)
            {
                int x = 0;
                while (request.arguments[i][x] != '\n')
                    x++;
                std::string tmp = request.arguments[i].substr(0, x - 1);
                for (size_t i = 0; i < this->_nicknames.size(); i++)
                {
                    if (this->_nicknames[i] == tmp)
                        return ("Nickname already taken\n");
                }
                this->client[fd]->setnickname(tmp);
                this->_nicknames.push_back(tmp);
            }
        if (i > 0 && request.arguments[i - 1].find("USER") != std::string::npos)
            {
                int x = 0;
                while (request.arguments[i][x] != '\n')
                    x++;
                std::string tmp = request.arguments[i].substr(0, x - 1);
                for (size_t i = 0; i < this->_usernames.size(); i++)
                {
                    if (this->_usernames[i] == tmp)
                        return ("Username already taken\n");
                }
                this->client[fd]->setusername(tmp);
                this->_usernames.push_back(tmp);
            }
    }
    if (this->client[fd] != NULL && this->client[fd]->getauthentification())
        return ("Welcome to the localnet IRC network " + this->client[fd]->getnickname() + "!001@" + this->ipAddress + "\n" + "Your username is " + this->client[fd]->getusername() + "\n");
    else
        return ("You are not connected\n");
}

std::string Server::connect(Request request, int fd)
{
    
    if (request.arguments.size() != 4)
         return ("Invalid number of arguments\n");
    if (request.arguments[1] != this->ipAddress)
        return ("Invalid ip address\n");
    if (request.arguments[2] != "8765")
        return ("Invalid port\n");
    if (request.arguments[3].find(this->password) == std::string::npos)
        return ("Invalid password\n");
   // if he is already connected
    if (this->client[fd] != NULL && this->client[fd]->getauthentification() == 1)
        return ("You are already connected\n");
    else
    {
        this->client[fd] = new Client(fd);
        this->client[fd]->setauthentification(1);
        std::string nickname = "Default";
        for (size_t i = 0; i < this->_nicknames.size(); i++)
        {
            if (this->_nicknames[i] == nickname)
                nickname += "._";
        }
        this->client[fd]->setnickname(nickname);
        this->_nicknames.push_back(nickname);
        std::string username = "Default";
        for (size_t i = 0; i < this->_usernames.size(); i++)
        {
            if (this->_usernames[i] == username)
                username += "._";
        }
        this->client[fd]->setusername(username);
        this->_usernames.push_back(username);
        return ("Welcome to the localnet IRC network " + this->client[fd]->getnickname() + "!001@" + this->ipAddress + "\n" + "Your username is " + this->client[fd]->getusername() + "\n");
    }
}

std::string Server::nick(Request request, int fd)
{
    if (client[fd] == NULL)
        return ("IRC : Not connected to the server\n");
    if (!this->client[fd]->getauthentification())
        {
            return ("IRC : Not connected to the server\n");
        }
    if (request.arguments.size() < 2)
        {
            return ("IRC : Your nickname is 'default'\n");
        }
    int i = 0;
    // printf("request.arguments[1]: %s\n", request.arguments[1].c_str());
    while (request.arguments[1][i] != '\0')
    { 
        if (request.arguments[1][i] == '-' && (request.arguments[1][i] < '0' || request.arguments[1][i] > '9'))
            return ("IRC : " + request.arguments[1] + " Erroneous Nickname\n");
        i++;
    }
    //check if nickname is already taken if he is already connected to the server return error
    for (size_t i = 0; i < this->_nicknames.size(); i++)
    {
        if (this->_nicknames[i] == request.arguments[1])
            return ("IRC : " + request.arguments[1] + " Nickname already taken\n");
    }
    this->client[fd]->setnickname(request.arguments[1]);
    this->_nicknames.push_back(request.arguments[1]);
    return ("IRC : Your nickname is " + request.arguments[1] + "\n");
}

std::string Server::user(Request request, int fd)
{
    if (client[fd] == NULL)
        return ("IRC : Not connected to the server\n");
    if (!this->client[fd]->getauthentification())
        {
            return ("IRC : Not connected to the server\n");
        }
    if (request.arguments.size() < 2)
        {
            return ("IRC : Your username is " + this->client[fd]->getusername() + "\n");
        }
    this->client[fd]->setusername(request.arguments[0]);
    return ("IRC : Your username is " + request.arguments[1]);
        // if (client->is_registered())
    // {
    //     client->reply(ERR_ALREADYREGISTERED(client->get_nickname()));
    //     return;
    // }

    // if (args.size() < 4) 
    // {
    //     client->reply(ERR_NEEDMOREPARAMS(client->get_nickname(), "USER"));
    //     return;
    // }

    // client->set_username(args[0]);
    // client->set_realname(args[3]);
    // client->welcome();
}

std::string Server::join(Request request, int fd)
{
    int i = 1;
    if (client[fd] == NULL || client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (request.arguments.size() < 2)
        return ("IRC : Not enough arguments\n");
    // join #channel #channel2 #channel3
    while (i < (int)request.arguments.size())
    {
        if (request.arguments[i][0] != '#')
            return ("IRC : Invalid channel name\n");
        if (request.arguments[i].length() > 200)
            return ("IRC : Channel name too long\n");
        if (this->client[fd]->getchannel() == request.arguments[i])
            return ("IRC : You are already in " + request.arguments[i] + "\n");
        else
        {
            std::map<std::string, Channel*>::iterator it = _channels.find(request.arguments[i]);
            if (it == _channels.end())
            {
                createChannel(request.arguments[i], fd);
            }
            else
            {
                //check +i or -i
                if (it->second->getinvite() == "+i")
                {
                    if (it->second->isMember(client[fd]) == false)
                        return ("IRC : You are not invited to " + request.arguments[i] + "\n");
                }
                else
                {
                    //check if the channel is full < limit_channel
                    if (it->second->getlimit() != 0 && it->second->getonlinemembers() >= it->second->getlimit())
                        return ("IRC : " + request.arguments[i] + " is full\n");
                    else
                    {
                        //if the channel have a key
                        if (it->second->get_key() != "")
                        {
                            if (request.arguments[i + 1] == "(null)" || request.arguments[i + 1] != it->second->get_key())
                                return ("IRC : Invalid key\n SORRY YOU CAN'T JOIN...THIS CHANNEL IS PROTECTED BY A KEY\n");
                            else if (request.arguments[i + 1] == it->second->get_key())
                            {
                                it->second->addMember(client[fd]);
                                it->second->update_onlinemembers();
                                this->client[fd]->setchannel(request.arguments[i]);
                                return (join_message(request.arguments[i], fd));
                            }
                        }
                        else
                        {
                            // it->second->addMember(client[fd]);
                            it->second->update_onlinemembers();
                            this->client[fd]->setchannel(request.arguments[i]);
                            return (join_message(request.arguments[i], fd));
                        }
                    }
                }
            }
        }
        i++;
    }
    return (join_message(request.arguments[i - 1], fd));
}

std::string Server::join_message(std::string channel, int fd)
{
    std::string response = "";
    response += this->client[fd]->getnickname() + " has joined " + channel + "\n";
    response += "Welcome to " + channel + " " + this->client[fd]->getnickname() + "\n";
    response += "Topic for " + channel + " is " + this->_channels[channel]->getTopic() + "\n";
    response += "Channel created on " + this->_channels[channel]->getCreationDate() + "\n";
    response += "There are " + this->_channels[channel]->intToString(this->_channels[channel]->getonlinemembers()) + " users on this channel\n";
    send_to_all_members(channel, response);
    return ("");
}

void Server::createChannel(std::string channel, int fd)
{
    this->_channels[channel] = new Channel(client[fd], channel);
    this->_channels[channel]->update_onlinemembers();
    this->client[fd]->setchannel(channel);
    this->_channels[channel]->setop(client[fd]);
}


std::string Server::mode(Request request, int fd)
{
    if (client[fd] == NULL)
        return ("IRC : Not connected to the server\n");
    if (client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (request.arguments.size() < 3)
        return ("IRC : Not enough arguments\n");
    if (this->client[fd]->getchannel() != request.arguments[1])
        return ("IRC : You are not in " + request.arguments[1] + "\n");
    if (request.arguments.size() < 3)
        return ("IRC : Not enough arguments\n");
    if (request.arguments[2].find("i") != std::string::npos)
    {
        //check first if the user is one of the operators
        if (this->_channels[request.arguments[1]]->isop(this->client[fd]) == false)
            return ("IRC : You are not an operator\n");
        else
        {
            if (request.arguments[2].find("+i") != std::string::npos)
            {
                this->_channels[request.arguments[1]]->invite_or_not("+i");
                return ("IRC : " + request.arguments[1] + " is now invite-only\n");
            }
            else if (request.arguments[2].find("-i") != std::string::npos)
            {
                this->_channels[request.arguments[1]]->invite_or_not("-i");
                return ("IRC : " + request.arguments[1] + " is public now -- anyone can join\n Welcome...\n");
            }
            else
                return ("IRC : Invalid mode\n");
        }
    }
    if (request.arguments[2].find("t") != std::string::npos)
    {
        if (this->_channels[request.arguments[1]]->isop(this->client[fd]) == false)
            return ("IRC : You are not an operator\n");
        else
        {
            if (request.arguments[2].find("+t") != std::string::npos)
                this->_channels[request.arguments[1]]->set_topic_or_not("+t");
            else if (request.arguments[2].find("-t") != std::string::npos)
                this->_channels[request.arguments[1]]->set_topic_or_not("-t");
            else
                return ("IRC : Invalid mode\n");
        }
    }
    if (request.arguments[2].find("o") != std::string::npos)
    {
        //! /mode #channel +/-o nickname
        if (this->_channels[request.arguments[1]]->isop(this->client[fd]) == false)
            return ("IRC : You are not an operator\n");
        else
        {
            if (request.arguments[2].find("+o") != std::string::npos)
                this->_channels[request.arguments[1]]->addoperator(request.arguments[3]);
            else if (request.arguments[2].find("-o") != std::string::npos)
                this->_channels[request.arguments[1]]->removeoperator(request.arguments[3]);
            else
                return ("IRC : Invalid mode\n");
        }
    }
    if (request.arguments[2].find("l") != std::string::npos)
    {
        if (this->_channels[request.arguments[1]]->isop(this->client[fd]) == false)
            return ("IRC : You are not an operator\n");
        else
        {
            if (request.arguments[2].find("+l") != std::string::npos)
            {
                this->_channels[request.arguments[1]]->setlimit(request.arguments[3]);
                return ("IRC : " + request.arguments[1] + " is now limited to " + request.arguments[3] + " users\n");
            }
            else if (request.arguments[2].find("-l") != std::string::npos)
            {
                this->_channels[request.arguments[1]]->setlimit("0");
                return ("IRC : " + request.arguments[1] + " is now unlimited to 4 users\n");
            }
            else
                return ("IRC : Invalid mode\n");
        }
    }
    if (request.arguments[2].find("k") != std::string::npos)
    {
        if (this->_channels[request.arguments[1]]->isop(this->client[fd]) == false)
            return ("IRC : You are not an operator\n");
        else
        {
            if (request.arguments[2].find("+k") != std::string::npos)
            {
                this->_channels[request.arguments[1]]->set_key(request.arguments[3]);
                return ("IRC : " + request.arguments[1] + " is now protected by a key\n");
            }
            else if (request.arguments[2].find("-k") != std::string::npos)
            {
                this->_channels[request.arguments[1]]->set_key("");
                return ("IRC : " + request.arguments[1] + " is now unprotected by a key\n");
            }
            else
                return ("IRC : Invalid mode\n");
        }
    }
    return ("");
}

std::string Server::topic(Request request, int fd)
{
    if (client[fd] == NULL || client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (this->client[fd]->getchannel() != request.arguments[1])
        return ("IRC : You are not in " + request.arguments[1] + "\n");
    if (request.arguments.size() < 3)
        return ("IRC : Not enough arguments\n");
    if (this->_channels[request.arguments[1]]->isop(this->client[fd]) == false)
        return ("IRC : You are not an operator\n");
    if (this->_channels[request.arguments[1]]->get_t() == false)
        return ("IRC : You are not allowed to change the topic\n");
    else
    {
        this->_channels[request.arguments[1]]->setTopic(request.arguments[2]);
        return ("IRC : " + request.arguments[1] + " topic is now " + request.arguments[2] + "\n");
    }
}

std::string Server::privmsg(Request request, int fd)
{
    if (client[fd] == NULL || client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (request.arguments.size() < 3)
        return ("IRC : Not enough arguments\n");
    // ? /privmsg nickname message
    // ! check if request.arguments[1] is connected ... channel is not important...
    for (std::map<int, Client*>::iterator it = this->client.begin(); it != this->client.end(); ++it)
    {
        if (it->second != NULL && it->second->getnickname() == request.arguments[1])
        {
            if (it->second->getauthentification() == 1)
            {
                std::string message = this->client[fd]->getnickname() + "==> ";

                for (size_t i = 2; i < request.arguments.size(); i++)
                {
                    message += request.arguments[i] + " ";
                }
                message += "\n";
                send(it->first, message.c_str(), message.size(), 0);
                return ("");
            }
        }
    }
    return ("IRC : " + request.arguments[1] + " is not connected\n");
}

std::string Server::invite(Request request, int fd)
{
    // * /invite nickname #channel
    if (client[fd] == NULL || client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (request.arguments.size() < 3)
        return ("IRC : Not enough arguments\n");
    if (this->client[fd]->getchannel() == "")
        return ("IRC : You are not in any channel\n");
    if (this->_channels[client[fd]->getchannel()]->isop(client[fd]) == false)
        return ("IRC : You are not an operator\n");
   // * request.arguments[1] is connected
    for (std::map<int, Client*>::iterator it = this->client.begin(); it != this->client.end(); ++it)
    {
        if (it->second != NULL && it->second->getauthentification() == 1 && it->second->getnickname() == request.arguments[1])
        {
            if (it->second->getchannel() != "")
                return ("IRC : " + request.arguments[1] + " is already in " + it->second->getchannel() + "\n");
            else
            {
                it->second->setchannel(request.arguments[2]);
                this->_channels[request.arguments[2]]->addMember(it->second);
                this->_channels[request.arguments[2]]->update_onlinemembers();
                send(it->first, "You have been invited to channel \n", 30, 0);
                join_message(request.arguments[2], it->first);
                return ("IRC : " + request.arguments[1] + " has joined " + request.arguments[2] + "\n");
            }
        }
    }
    return ("IRC : " + request.arguments[1] + " is not connected\n");
}

std::string Server::kick(Request request, int fd)
{
    // * /kick #channel nickname
    if (client[fd] == NULL || client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (request.arguments.size() < 3)
        return ("IRC : Not enough arguments\n");
    if (this->client[fd]->getchannel() == "" || this->client[fd]->getchannel() != request.arguments[1])
        return ("IRC : You are not in any channel\n");
    if (this->_channels[client[fd]->getchannel()]->isop(client[fd]) == false)
        return ("IRC : You are not an operator\n");
    for (std::map<int, Client*>::iterator it = this->client.begin(); it != this->client.end(); ++it)
    {
        if (it->second != NULL && it->second->getauthentification() == 1 && it->second->getnickname() == request.arguments[2])
        {
            if (it->second->getchannel() == request.arguments[1])
            {
                it->second->setchannel("");
                this->_channels[request.arguments[1]]->removeMember(it->second);
                this->_channels[request.arguments[1]]->update_onlinemembers();
                send(it->first, "You have been kicked \n", 18, 0);
                return ("IRC : " + request.arguments[2] + " has been kicked from " + request.arguments[1] + "\n");
            }
            else
                return ("IRC : " + request.arguments[2] + " is not in " + request.arguments[1] + "\n");
        }
    }
    return ("IRC : " + request.arguments[2] + " is not connected\n");
}

std::string Server::quit(Request request, int fd)
{
    //* /quit from the server
    (void)request;
    if (client[fd] == NULL || client[fd]->getauthentification() == 0)
        return ("IRC : Not connected to the server\n");
    if (this->client[fd]->getchannel() != "")
    {
        this->_channels[this->client[fd]->getchannel()]->removeMember(this->client[fd]);
        this->_channels[this->client[fd]->getchannel()]->update_onlinemembers();
    }
    this->client[fd]->setauthentification(0);
    this->client[fd]->setchannel("");
    this->client[fd]->setnickname("");
    this->client[fd]->setusername("");
    return ("IRC : Goodbye\n");
}
