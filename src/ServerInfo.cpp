#include "ServerInfo.hpp"
#include "LocationInfo.hpp"
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <vector>

ServerInfo::ServerInfo() : port(0), _client_max_body_size(0), autoindex(false) {}

ServerInfo::~ServerInfo()
{
	for (std::vector <LocationInfo *>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		delete *it;
	}
}


ServerInfo::ServerInfo(int tfd, struct sockaddr_storage ss, size_t taddr_len):
	Node(tfd, ss, taddr_len, SERVER_NODE)
{}

ServerInfo::ServerInfo(const ServerInfo& rhs) : Node()
{*this = rhs;}

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

std::string	ServerInfo::get_root() const
{
	return (root);
}

void	ServerInfo::set_root(const std::string& rt)
{
	root = rt;
}

int	ServerInfo::get_port() const
{
	return port;
}

void	ServerInfo::set_port(int prt)
{
	port = prt;
}

void	ServerInfo::set_server_name(const std::vector <std::string>& tserver_name)
{
	server_name = tserver_name;
}

std::vector <std::string>	ServerInfo::get_server_name() const
{
	return server_name;
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

struct in_addr		ServerInfo::get_host_address() const
{
	return _host_address;
}

void	ServerInfo::set_host_address(struct in_addr& host)
{
	_host_address = host;
}

int	ServerInfo::get_client_max_body_size() const
{
	return _client_max_body_size;
}

void	ServerInfo::set_client_max_body_size(const int client_max_body_size)
{
	_client_max_body_size= client_max_body_size;
}

void	ServerInfo::add_locations(std::vector <LocationInfo*> locations)
{
	this->locations = locations;
}

std::vector <LocationInfo*>	ServerInfo::get_locations() const
{
	return locations;
}

std::ostream &operator<<(std::ostream& os, const ServerInfo& rhs)
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