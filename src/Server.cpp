#include "Server.hpp"

Server::Server()
{}

Server::~Server()
{}

Server&	operator=(const Server& rhs)
{
	if (this != &rhs)
	{

	}
	return (*this);
}

Server::Server(const Server& rhs)
{
	*this = rhs;
}

Request	Server::get_request() const
{
	return request;
}

ServerInfo	Server::get_server_info() const
{
	return server_info;
}

void	Server::respond(Request rq)
{
	if (rq.hasError())
	{
		
	}
}