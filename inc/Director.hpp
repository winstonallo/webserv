#ifndef DIRECTOR_HPP
#define DIRECTOR_HPP

#include <ctime>
#include <map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <string>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include "Node.hpp"
#include "Server.hpp"
#include "ClientInfo.hpp"
#include "Config.hpp"

#define MSG_SIZE 20000
#define TIMEOUT_TIME 5

// purpose:	class that handles all connections wit select(ing) the 
// 			the sockets that are ready to be readed or written.
class Config;

class Director 
{
	public:

		struct TimeoutInfo
		{
			time_t last_activity;
			ClientInfo* client;
		};

										Director(const std::string& path);
										~Director();

		int								init_servers();
		int								run_servers();

	private:
										Director(const Director&) {}
		Director&						operator=(const Director&) { return *this; }
		void							*get_in_addr(struct sockaddr *sa);
		int								init_server(Server *server);
		int								create_client_connection(int listener);
		int								read_from_client(int fd);
		int								write_to_client(int fd);
		void							close_cgi(ClientInfo* client, int status_code);

		void							close_timed_out_clients();
		std::vector <int>				get_timed_out_clients();
		void							close_client_connection(int client_fd, const std::string& message="");
		void							send_timeout_response(int client_fd, ClientInfo* client);
		void							cgi_timeout(int client_fd, ClientInfo* client);

		void							remove_client(int client_fd, ClientInfo* client=NULL);
		void							clear_file_descriptor(int client_fd, bool close_fd=true);
		void							add_file_descriptor(int fd, fd_set& set);

		bool							is_same_socket(std::vector<Server*>::iterator it, std::vector<Server*>::iterator sub_it);

		int								_fdmax;
		Config*							_config;
		std::map <int, TimeoutInfo>		_client_timeouts;
		std::map <int, Node*>			_nodes;
		fd_set							_read_fds;
		fd_set							_write_fds;
};

#endif
