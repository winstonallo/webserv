#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <iostream>
#include <string> 
#include <vector>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Node.hpp"
#include "LocationInfo.hpp"

class LocationInfo;

//purpose:	Data class for the parsed values (from the config file)
//			It is used for the initialization and the running of the servers.

class ServerInfo : public Node
{
	public:

											ServerInfo();
											~ServerInfo();
											ServerInfo(const ServerInfo& rhs);
											ServerInfo(int tfd, struct sockaddr_storage ss, size_t addr_len);
		ServerInfo&							operator=(const ServerInfo& rhs);

		// getters and setters
		int									get_port() const;
		void								set_port(int prt);
		int									get_sock_fd() const;
		void								set_sock_fd(int sfd);
		std::string							get_server_name() const;
		void								set_server_name(const std::string& tserver_name);
		bool								get_auto_index() const;
		void								set_auto_index(bool b);
		std::string							get_root() const;
		void								set_root(const std::string& rt);
		std::string							get_error_log() const;
		void								set_error_log(const std::string& rt);
		std::string							get_access_log() const;
		void								set_access_log(const std::string& rt);
		LocationInfo&						get_location(int);
		void								add_location(LocationInfo lc);
		bool								is_fd_in_clients(int fd) const;

	private:

		int									port;
		std::string							server_name;
//		struct sockaddr_in					address;
		bool								autoindex;
		std::string							root;
		std::string							error_log;
		std::string							access_log;
		std::vector<LocationInfo>			locations;
};

#endif