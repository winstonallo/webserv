/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:46:26 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:33:37 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"


ServerConfig::ServerConfig()
{

}

ServerConfig::~ServerConfig()
{

}

ServerConfig::ServerConfig(const ServerConfig& rhs)
{
	*this = rhs;
}

ServerConfig::ServerConfig(const int& tport, const std::string& tserver_name):port(tport), server_name(tserver_name)
{

}

ServerConfig&	ServerConfig::operator=(const ServerConfig& rhs)
{
	if (this != &rhs)
	{
		port = rhs.port;
		server_name = rhs.server_name;
	}
	return (*this);
}

int	ServerConfig::getPort() const
{
	return (port);
}

void	ServerConfig::setPort(int prt)
{
	port = prt;
}

std::string	ServerConfig::getServerName() const
{
	return (server_name);
}

void	ServerConfig::setServerName(const std::string& tserver_name)
{
	server_name = tserver_name;
}

std::string	ServerConfig::getErrorLog() const
{
	return (error_log);
}

void	ServerConfig::setErrorLog(const std::string& log)
{
	error_log = log;
}

std::string	ServerConfig::getAccessLog() const
{
	return (access_log);
}

void	ServerConfig::setAccessLog(const std::string& log)
{
	access_log = log;
}

LocationConfig&	ServerConfig::getLocation(int i) const
{
	return (locations(i));
}

void	ServerConfig::addLocation(LocationConfig lc)
{
	locations.push_back(lc);
}








