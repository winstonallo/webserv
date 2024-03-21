#ifndef SERVERINFO_HPP
#define SERVERINFO_HPP

#include <string> 
#include <vector>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Node.hpp"
#include "Server.hpp"
#include "LocationInfo.hpp"

class LocationInfo;

//purpose:	Data class for the parsed values (from the config file)
//			It is used fjor the initialization and the running of the servers.
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
		std::vector <std::string>			get_server_name() const;
		void								set_server_name(const std::vector <std::string>& tserver_name);
		bool								get_auto_index() const;
		void								set_auto_index(bool b);
		std::string							get_root() const;
		void								set_root(const std::string& rt);
		std::string							get_error_log() const;
		void								set_error_log(const std::string& rt);
		std::string							get_access_log() const;
		void								set_access_log(const std::string& rt);
		struct in_addr						get_host_address() const;
		void								set_host_address(struct in_addr& host);
		int									get_client_max_body_size() const;
		void								set_client_max_body_size(const int client_max_body_size);
		LocationInfo&						get_location(int);
		void								add_locations(std::vector <LocationInfo *> locations);
		bool								is_fd_in_clients(int fd) const;
		Server								server;

	private:

		int									port;
		int									_client_max_body_size;
		std::vector <std::string>			server_name;
		struct in_addr						_host_address;
//		struct sockaddr_in					address;
		bool								autoindex;
		std::string							root;
		std::string							error_log;
		std::string							access_log;
		std::map<int, std::string>			error_pages;
		std::vector<LocationInfo>			locations;
};

#endif