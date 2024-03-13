#include "../inc/Director.hpp"

Director::Director():fdmax(-1)
{}

Director::~Director()
{}

Director::Director(const Director& rhs)
{
	*this = rhs;
}

Director&	Director::operator=(const Director& rhs)
{
	if (this != &rhs)
	{}
	return (*this);
}

void	Director::add_server_info(ServerInfo si)
{
	server_infos.push_back(si);
}

void*	Director::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr);
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int	Director::init_server(ServerInfo *si)
{
	struct addrinfo hints, *ai, *p;
	int listener;
	int rv, yes=1 ;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::stringstream pt;
	pt << si->get_port();
	if ((rv = getaddrinfo(NULL, pt.str().c_str(), &hints, &ai)) != 0) 
	{
		std::stringstream ss;
		ss << "Error reading addrinfo: " << gai_strerror(rv) << std::endl;
		Log::log(ss.str(), ERROR_FILE | STD_ERR);
		return -1;
	}
	for (p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
		{
			std::stringstream ss;
			ss << "Error opening socket: " << strerror(errno) << std::endl;	
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			continue;
		}
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			std::stringstream ss;
			ss << "Bind error: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			continue;
		}
		si->set_fd(listener);
		si->set_addr(*((sockaddr_storage*)(p->ai_addr)));
		si->set_addr_len((size_t)p->ai_addrlen);
		break;
	}

	if (p == NULL)
	{
		std::stringstream ss;
		ss << "Select server failed to bind." << std::endl;
		Log::log(ss.str(), ERROR_FILE | STD_ERR);
		return -1;
	}
	si->set_fd(listener);
	freeaddrinfo(ai);
	return 0;
}

int	Director::init_servers()
{
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	std::vector<ServerInfo>::iterator e = server_infos.end();
	std::vector<ServerInfo>::iterator it ;
	for (it = server_infos.begin() ; it != e; it++)
	{
		if (init_server(&(*it)) < 0)
			return -1;
		int listener = it->get_fd();
		if (fcntl(listener, F_SETFL, O_NONBLOCK) < 0)
		{
			std::stringstream ss;
			ss << "Error unblocking socket: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		if (listen(listener, 800) == -1)
		{
			std::stringstream ss;
			ss << "Error listening: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		FD_SET(listener, &read_fds);
		if (fdmax < listener) 
			fdmax = listener;
		nodes[listener] = &(*it);
		nodes[listener]->set_type(SERVER_NODE);
		nodes[listener]->set_fd(listener);
	}
	return 0;
}

int	Director::run_servers()
{
	int ret;
	fd_set readfds_backup;
	fd_set writefds_backup;
	struct timeval timeout_time;
	while (true)
	{
		readfds_backup = read_fds;
		writefds_backup = write_fds;
		timeout_time.tv_sec = 5;
		timeout_time.tv_usec = 0;
		if ((ret = select(fdmax + 1, &readfds_backup, &writefds_backup, NULL, &timeout_time)) < 0 )
		{
			std::stringstream ss;
			ss << "Error while select: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		for (int i = 0; i <= fdmax; i++)
		{
			if (nodes.find(i) != nodes.end() && nodes[i]->get_type() == SERVER_NODE)
			{
				if (FD_ISSET(i, &readfds_backup))
					if(create_client_connection(i) < 0)
					{
						std::stringstream ss;
						ss << "Error creating a client connection: " << std::endl;
						Log::log(ss.str(), ERROR_FILE | STD_ERR);
						exit(2);
					}
				
			}
			else if (nodes.find(i) != nodes.end() && nodes[i]->get_type() == CLIENT_NODE) 
			{
				if (FD_ISSET(i, &readfds_backup))
				{
					if(read_from_client(i) < 0)
					{
						std::stringstream ss;
						std::cerr << "Error reading from client: " << std::endl;
						Log::log(ss.str(), ERROR_FILE | STD_ERR);
					}
				}
				if (FD_ISSET(i, &write_fds))
				{
					if(write_to_client(i) < 0)
					{
						std::stringstream ss;
						std::cerr << "Error writing to client." << std::endl;
						Log::log(ss.str(), ERROR_FILE | STD_ERR);
					}
				}
			}
		}
	}
	return 0;
}

int	Director::create_client_connection(int listener)
{
	struct sockaddr_storage remoteaddr;
	socklen_t 				addrlen = sizeof remoteaddr;
	char					remoteIP[INET6_ADDRSTRLEN];	

	int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
	{
		std::stringstream	ss;
		ss << "Error accepting client: " << strerror(errno) << std::endl;
		Log::log(ss.str(), ERROR_FILE | STD_ERR);
		return -1;
	}
	else
	{
		if (fdmax < newfd)
			fdmax = newfd;
		if (nodes.find(newfd) == nodes.end())
		{
			nodes[newfd] = new ClientInfo(newfd, remoteaddr, (size_t)addrlen);
		}
		else
		{
			std::cerr << "Tried to overwrite socket: " << newfd << std::endl;
			exit(2);
		}
		std::stringstream ss2;

		ss2 << "New connection from ";
		ss2 << inet_ntop(remoteaddr.ss_family,
						get_in_addr((struct sockaddr *)&remoteaddr),
						remoteIP, INET6_ADDRSTRLEN);
		ss2 << " on socket " << newfd << std::endl;
		Log::log(ss2.str(), ACCEPT_FILE | STD_OUT);
		if (fcntl(newfd, F_SETFL, O_NONBLOCK) < 0)
		{
			std::stringstream ss3;
			ss3 << "Error while non-blocking: " << strerror(errno) << std::endl;
			Log::log(ss3.str(), ERROR_FILE | STD_ERR);
			delete nodes[newfd];
			nodes.erase(newfd);
			close(newfd);
		}
		FD_SET(newfd, &read_fds);
	}
	return 0;
}

int	Director::read_from_client(int client_fd)
{
	char	msg[MSG_SIZE];
	memset(&msg, 0, MSG_SIZE);
	int		num = read(client_fd, msg, MSG_SIZE);
	if (!num)
	{
		std::stringstream ss;
		ss << "Connection closed by client " << client_fd << std::endl;
		Log::log(ss.str(), ACCEPT_FILE | STD_OUT);
		if (FD_ISSET(client_fd, &write_fds))
			FD_CLR(client_fd, &write_fds);
		if (FD_ISSET(client_fd, &read_fds))
			FD_CLR(client_fd, &read_fds);
		if (client_fd == fdmax)
			fdmax--;
		delete nodes[client_fd];
		nodes.erase(client_fd);
		close(client_fd);
	}
	else
	{
		if (num == -1)
		{
			std::stringstream ss;
			ss << "Error reading from socket: " << client_fd << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;	
		}
		std::cout << msg;
	}
	return 0;
}

int	Director::write_to_client(int fd)
{
	(void)fd;
	return 0;
}
