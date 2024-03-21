#ifndef DIRECTOR_HPP
#define DIRECTOR_HPP

#include <map>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <cerrno>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include "Node.hpp"
#include "Request.hpp"
#include "ServerInfo.hpp"
#include "Server.hpp"
#include "ClientInfo.hpp"
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Log.hpp"
#include "LocationInfo.hpp"

#define MSG_SIZE 50000
#define TIMEOUT_TIME 30

// purpose:	class that handles all connections wit select(ing) the 
// 			the sockets that are ready to be readed or written.


class Director 
{
	public:
										Director(const std::string& path);
										~Director();
		int								init_servers();
		int								run_servers();

	private:
										Director(const Director& rhs);
		Director&						operator=(const Director& rhs);
		void							*get_in_addr(struct sockaddr *sa);
		int								init_server(ServerInfo *si);
		int								create_client_connection(int listener);
		int								read_from_client(int fd);
		int								write_to_client(int fd);

		int								fdmax;
		Config							config;
		std::map<int, Node*>			nodes;
		fd_set							read_fds, write_fds;
};

#endif
