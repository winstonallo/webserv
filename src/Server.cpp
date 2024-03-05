#include "../inc/Server.hpp"

Server::Server() {}

Server::~Server() {}

Server::Server(const Server& rhs) {*this = rhs;}

Server&	Server::operator=(const Server& rhs)
{
	if (this != &rhs)
	{
		port = rhs.port;
		server_name = rhs.server_name;
	}
	return (*this);
}

int	Server::getPort() const
{
	return (port);
}

void	Server::setPort(int prt)
{
	port = prt;
}

std::string	Server::getServerName() const
{
	return (server_name);
}

void	Server::setServerName(const std::string& tserver_name)
{
	server_name = tserver_name;
}

std::string	Server::getErrorLog() const
{
	return (error_log);
}

void	Server::setErrorLog(const std::string& log)
{
	error_log = log;
}

std::string	Server::getAccessLog() const
{
	return (access_log);
}

void	Server::setAccessLog(const std::string& log)
{
	access_log = log;
}
