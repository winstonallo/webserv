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
#include "Node.hpp"
#include "Server.hpp"
#include "ClientInfo.hpp"
#include "Config.hpp"

#define MSG_SIZE 20000
#define TIMEOUT_TIME 30

// purpose:	class that handles all connections wit select(ing) the 
// 			the sockets that are ready to be readed or written.
class Config;

class Director 
{
	public:
		typedef std::map<int, std::pair <Node*, time_t> > timeout_node_map;

										Director(const std::string& path);
										~Director();
		int								init_servers();
		int								run_servers();
		Config*							get_config();

	private:
										Director(const Director& rhs);
		Director&						operator=(const Director& rhs);
		void							*get_in_addr(struct sockaddr *sa);
		int								init_server(Server *si);
		int								create_client_connection(int listener);
		int								read_from_client(int fd);
		int								write_to_client(int fd);

		int								fdmax;
		Config*							config;
		timeout_node_map				_nodes;
		fd_set							read_fds, write_fds;
};

#endif
