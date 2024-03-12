#include "../inc/ServerInfo.hpp"

ServerInfo::ServerInfo() {}

ServerInfo::~ServerInfo() {}


ServerInfo::ServerInfo(int tfd, struct sockaddr_storage ss, size_t taddr_len):
	Node(tfd, ss, taddr_len, SERVER_NODE)
{}

ServerInfo::ServerInfo(const ServerInfo& rhs) {*this = rhs;}

ServerInfo&	ServerInfo::operator=(const ServerInfo& rhs)
{
	if (this != &rhs)
	{
		port = rhs.port;
		server_name = rhs.server_name;
//		address = rhs.address;
		autoindex = rhs.autoindex;
		root = rhs.root;
		error_log = rhs.error_log;
		access_log = rhs.access_log;
		locations = rhs.locations;
	}
	return (*this);
}

int	ServerInfo::get_port() const
{
	return (port);
}

void	ServerInfo::set_port(int prt)
{
	port = prt;
}

std::string	ServerInfo::get_server_name() const
{
	return (server_name);
}

void	ServerInfo::set_server_name(const std::string& tserver_name)
{
	server_name = tserver_name;
}

bool	ServerInfo::get_auto_index() const
{
	return (autoindex); 
}

void	ServerInfo::set_auto_index(bool t)
{
	autoindex = t;
}

std::string	ServerInfo::get_error_log() const
{
	return (error_log);
}

void	ServerInfo::set_error_log(const std::string& log)
{
	error_log = log;
}

std::string	ServerInfo::get_access_log() const
{
	return (access_log);
}

void	ServerInfo::set_access_log(const std::string& log)
{
	access_log = log;
}