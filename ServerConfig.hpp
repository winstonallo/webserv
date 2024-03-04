/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:46:26 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:20:46by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
#define SERVERCONFIG_HPP

#include <iostream>
#include <string>
#include <vector>
#include "LocationConfig.hpp"

class ServerConfig 
{
	public:
											ServerConfig();
											~ServerConfig();
											ServerConfig(const ServerConfig& rhs);
											ServerConfig(const int& tport, const std::string& tserver_name);
		ServerConfig&						operator=(const ServerConfig& rhs);
		

		int									getPort() const;
		void								setPort(int prt);
		std::string							getServerName() const;
		void								setServerName(const std::string& tserver_name);
		std::string							getRoot() const;
		void								setRoot(const std::string& rt);
		std::string							getErrorLog() const;
		void								setErrorLog(const std::string& rt);
		std::string							getAccessLog() const;
		void								setAccessLog(const std::string& rt);
		LocationConfig&						getLocation(int);
		void								addLocation(LocationConfig lc);

	private:
		int									port;
		std::string							server_name;
		std::string							root;
		std::vector<LocationConfig>			locations;
		std::string							error_log;
		std::string							access_log
};

#endif
