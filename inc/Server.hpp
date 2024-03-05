#ifndef SERVER_HPP
#define SERVER_HPP
#pragma once

#include "Headers.hpp"

class Server
{
	public:

		Server();
		~Server();

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
		// LocationConfig&						getLocation(int);
		// void								addLocation(LocationConfig lc);

	private:

		int									port;
		std::string							server_name;
		std::string							root;
		std::string							error_log;
		std::string							access_log;
		// std::vector<LocationConfig>			locations;

		Server(const Server& rhs);
		Server& operator=(const Server& rhs);
};

#endif