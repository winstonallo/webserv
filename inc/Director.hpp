#ifndef DIRECTOR_HPP
#define DIRECTOR_HPP

#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include "ClientInfo.hpp"
#include "Log.hpp"
#include "ServerInfo.hpp"

#define MSG_SIZE 50000

class Director 
{
	public:
										Director();
										~Director();
		void							add_server_info(ServerInfo si);
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

		std::vector<ServerInfo>			server_infos;
		std::map<int, Node*>			nodes;
		fd_set							read_fds, write_fds;
		int								fdmax;
};

#endif
