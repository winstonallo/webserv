#include "Server.hpp"
#include "LocationInfo.hpp"
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <vector>

Server::Server() : port(0), _client_max_body_size(0), autoindex(false) {}

Server::~Server()
{
	for (std::vector <LocationInfo *>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		delete *it;
	}
}


Server::Server(int tfd, struct sockaddr_storage ss, size_t taddr_len):
	Node(tfd, ss, taddr_len, SERVER_NODE)
{}

Server::Server(const Server& rhs) : Node()
{*this = rhs;}

Server&	Server::operator=(const Server& rhs)
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

std::string	Server::get_root() const
{
	return (root);
}

void	Server::set_root(const std::string& rt)
{
	root = rt;
}

int	Server::get_port() const
{
	return port;
}

void	Server::set_port(int prt)
{
	port = prt;
}

void	Server::set_server_name(const std::vector <std::string>& tserver_name)
{
	server_name = tserver_name;
}

std::vector <std::string>	Server::get_server_name() const
{
	return server_name;
}

bool	Server::get_auto_index() const
{
	return (autoindex); 
}

void	Server::set_auto_index(bool t)
{
	autoindex = t;
}

std::string	Server::get_error_log() const
{
	return (error_log);
}

void	Server::set_error_log(const std::string& log)
{
	error_log = log;
}

std::string	Server::get_access_log() const
{
	return (access_log);
}

void	Server::set_access_log(const std::string& log)
{
	access_log = log;
}

struct in_addr		Server::get_host_address() const
{
	return _host_address;
}

void	Server::set_host_address(struct in_addr& host)
{
	_host_address = host;
}

int	Server::get_client_max_body_size() const
{
	return _client_max_body_size;
}

void	Server::set_client_max_body_size(const int client_max_body_size)
{
	_client_max_body_size= client_max_body_size;
}

void	Server::add_locations(std::vector <LocationInfo*> locations)
{
	this->locations = locations;
}

std::vector <LocationInfo*>	Server::get_locations() const
{
	return locations;
}

std::ostream &operator<<(std::ostream& os, const Server& rhs)
{
	os << "\tport: " << rhs.get_port() << std::endl;
	os << "\tserver_name: ";
	for (size_t i = 0; i < rhs.get_server_name().size(); i++)
	{
		os << rhs.get_server_name()[i] << " ";
	}
	os << std::endl;
	os << "\troot: " << rhs.get_root() << std::endl;
	os << "\terror_log: " << rhs.get_error_log() << std::endl;
	os << "\taccess_log: " << rhs.get_access_log() << std::endl;
	os << "\thost_address: " << inet_ntoa(rhs.get_host_address()) << std::endl;
	os << "\tclient_max_body_size: " << rhs.get_client_max_body_size() << std::endl;
	os << "\tlocations: " << std::endl;
	for (size_t i = 0; i < rhs.get_locations().size(); i++)
	{
		os << "\t" << *rhs.get_locations()[i] << std::endl;
	}
	return os;
}