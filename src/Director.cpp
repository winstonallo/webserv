#include "Director.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <ctime>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <map>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <vector>
#include <signal.h>
#include "ClientInfo.hpp"
#include "Log.hpp"
#include "Node.hpp"
#include "Server.hpp"
#include "Utils.hpp"

Director::Director(const std::string& config_path) : _fdmax(-1)
{
	_config = new Config(config_path);
}

Director::~Director()
{
	std::map <int, Node*>::iterator it;

	for (it = _nodes.begin(); it != _nodes.end(); it++)
	{
		if (it->second)
		{
			close(it->second->get_fd());
		}
		if (it->second && it->second->get_type() != SERVER_NODE)
		{
			delete it->second;
		}
	}
	delete _config;
}

// purpose: gets the inner address of sockaddr sa for both cases that 
// 			its a IPv4 and IPv6 address.
//
// argument: sa -> pointer to the given sockaddr of the addrinfo
//
// return void* -> pointer to the inner address.
void*	Director::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
	{
		return &(((struct sockaddr_in *)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// purpose: Initializes the Server that is passed in with Server*.
// 			We loop through the addreses of the Server and create for the first one 
//			a listener socket, which we set to be port-reusable and bind it to a port.
// 			we also save information to the Server (file descriptor, address)
//
// argument: si -> pointer to the Server information which we 
// 			 got from config parsing.
//
// return: int -> -1 if there was an error 0 if successfull
int	Director::init_server(Server *server)
{
	server->set_director(this);
	struct addrinfo hints;
	struct addrinfo *address_info;
	struct addrinfo *p;
	int listener;
	int rv, yes=1 ;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	std::stringstream port;
	port << server->get_port();
	if ((rv = getaddrinfo(NULL, port.str().c_str(), &hints, &address_info)) != 0) 
	{
		throw std::runtime_error(
			"Error: Could not read address info: " +
			std::string(gai_strerror(rv)) +
			".\n"
		);
	}
	for (p = address_info; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
		{
			Log::log(
				"Error: Could not open socket: " +
				std::string(strerror(errno)) +
				".\n",
				ERROR_FILE | STD_ERR
			);
			continue ;
		}

		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			Log::log(
				"Error: Could not bind: " +
				std::string(strerror(errno)) +
				".\n",
				ERROR_FILE | STD_ERR
			);
			close(listener);
			continue ;
		}

		server->set_fd(listener);
		server->set_addr_len((size_t)p->ai_addrlen);

		Log::log("Server created on localhost with domain name: " +
			server->get_server_name()[0] + ", port: " +
			Utils::itoa(server->get_port()) + "\n",
		ACCEPT_FILE | STD_OUT
		);

		break ;
	}

	if (p == NULL)
	{
		freeaddrinfo(address_info);
		throw std::runtime_error("Error: Select server failed to bind.\n");
	}

	_fdmax--;
	server->set_fd(listener);
	freeaddrinfo(address_info);
	return 0;
}

bool Director::is_same_socket(std::vector<Server*>::iterator it, std::vector<Server*>::iterator sub_it)
{
	return ((*sub_it)->get_host_address().s_addr == (*it)->get_host_address().s_addr &&
		(*sub_it)->get_port() == (*it)->get_port()
		);
}

// purpose: Take the Servers which we got from Config parsing
// 			and Initializes each one of them, set them so they don't block
// 			and makes them listen. The sockets are but in the read set for 
// 			the select method. We also add the server to the map of nodes 
// 			which is used for iterating through them.
//
// return: int -> -1 if there was an error 0 if successfull
int	Director::init_servers()
{
	FD_ZERO(&_read_fds);
	FD_ZERO(&_write_fds);

	std::vector<Server*> 			servers = _config->get_servers();
	std::vector<Server*>::iterator 	e = servers.end();
	std::vector<Server*>::iterator 	it;
	std::vector<Server*>::iterator 	sub_it;
	bool							same_socket;	

	for (it = servers.begin(); it != e; it++)
	{
		same_socket = false;
		for (sub_it = servers.begin(); sub_it != it; sub_it++) 
		{
			if ((same_socket = is_same_socket(it, sub_it)))
			{
				(*it)->set_fd((*sub_it)->get_fd());
			}
		}
		if (same_socket == false)
		{
			init_server(*it);
		}
		else
		{
			(*it)->set_director(this);
		}
	}

	for (it = servers.begin(); it != e; it++)
	{
		int listener = (*it)->get_fd();
		if (fcntl(listener, F_SETFL, O_NONBLOCK) < 0)
		{
			throw std::runtime_error(
				"Error: Could not unblock socket: " +
				std::string(strerror(errno)) + ".\n"
			);
		}
		if (listen(listener, 512) == -1)
		{
			throw std::runtime_error(
				"Error: Could not listen: " +
				std::string(strerror(errno)) + ".\n"
			);
		}

		add_file_descriptor(listener, _read_fds);
		_nodes[listener] = *it;
		_nodes[listener]->set_type(SERVER_NODE);
		_nodes[listener]->set_fd(listener);
	}
	return 0;
}

void	Director::add_file_descriptor(int fd, fd_set& set)
{
	FD_SET(fd, &set);

	if (_fdmax < fd)
	{
		_fdmax = fd;
	}
}

extern bool interrupted;

// purpose: In a loop we poll (with select) the statuses of the sockets.
// 			if they are ready for reading they create a new client sockets (if 
// 			its a server) and handle incoming or outgoing messages (if it's a client)
// 			we have a timeout of 1sec for the select(maybe we should modify)
//			and a timeout for the clients, if they are idle more then TIMEOUT_TIME
//
// return: int -> -1 if there was an error 0 if successfull
int	Director::run_servers()
{
	int 						ret;
	fd_set 						readfds_backup;
	fd_set 						writefds_backup;
	struct timeval 				timeout_time;
	ClientInfo* 				client;


	while (interrupted == false)
	{
		readfds_backup = _read_fds;
		writefds_backup = _write_fds;
		timeout_time.tv_sec = 1;
		timeout_time.tv_usec = 0;

		if ((ret = select(_fdmax + 1, &readfds_backup, &writefds_backup, NULL, &timeout_time)) < 0 )
		{
			if (interrupted == true)
			{
				break ;
			}

			std::stringstream ss;
			ss << "Error while select: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		for (int i = 0; i <= _fdmax; i++)
		{
			if (_nodes.find(i) != _nodes.end())
			{
				if (_nodes[i] != NULL and _nodes[i]->get_type() == SERVER_NODE)
				{
					if (FD_ISSET(i, &readfds_backup))
					{
						try
						{
							create_client_connection(i);
						}
						catch(const std::exception& e)
						{
							Log::log(e.what(), STD_ERR | ERROR_FILE);
						}
					}
				}
				else if (_nodes[i] != NULL and _nodes[i]->get_type() == CLIENT_NODE)
				{
					client = static_cast<ClientInfo*>(_nodes[i]);
					if (FD_ISSET(i, &readfds_backup))
					{
						try
						{
							read_from_client(i);
						}
						catch(const std::exception& e)
						{
							Log::log("Error reading from client: " + std::string(e.what()) + "\n", STD_ERR | ERROR_FILE);
						}
					}
					if (FD_ISSET(i, &writefds_backup))
					{
						if (client->is_cgi() && FD_ISSET(client->get_cgi()->request_fd[1], &writefds_backup) && !client->get_fin())
						{
							int send;

							std::string&	reqb = client->get_request()->get_body();
							if (!reqb.size())
							{
								send = 0;
							}
							else if (reqb.size() >= MSG_SIZE)
							{
								send = write(client->get_cgi()->request_fd[1], reqb.c_str(), MSG_SIZE);
							}
							else
							{
								send = write(client->get_cgi()->request_fd[1], reqb.c_str(), reqb.size());
							}
							if (send < 0)
							{
								std::stringstream ss;
								ss << "Error sending request body to CGI: " << strerror(errno);
								Log::log(ss.str(), STD_ERR | ERROR_FILE);
								close_cgi(client, 500);
							}
							else if (send == 0 || (size_t) send == reqb.size())
							{
								clear_file_descriptor(client->get_cgi()->response_fd[1]);
								clear_file_descriptor(client->get_cgi()->request_fd[1]);
							}
							else
							{
								reqb = reqb.substr(send);
							}
						}
						else if (client->get_cgi() && FD_ISSET(client->get_cgi()->response_fd[0], &readfds_backup) && !client->get_fin())
						{
							char	msg[MSG_SIZE * 4];
							int		receive = 0;
							int		status = 0;

							receive = read(client->get_cgi()->response_fd[0], msg, MSG_SIZE * 4);
							if (receive == 0)
							{
								clear_file_descriptor(client->get_cgi()->response_fd[0]);
								waitpid(client->get_pid(), &status, WNOHANG);
								if (WEXITSTATUS(status) != 0)
								{
									client->get_request()->set_errcode(500);
									client->get_server()->create_response(*client->get_request(), client);
								}
								if (client->get_response().find("HTTP/1.1") == std::string::npos)
								{
									client->get_response().insert(0, "HTTP/1.1 200 OK\r\n");
								}
								client->set_fin(true);
							}
							else if (receive < 0)
							{
								std::stringstream ss;
								ss << "Error reading CGI response: " << strerror(errno);
								Log::log(ss.str(), STD_ERR | ERROR_FILE);
								clear_file_descriptor(client->get_cgi()->response_fd[0]);
								clear_file_descriptor(client->get_cgi()->request_fd[0]);
								client->get_request()->set_errcode(500);
								client->get_server()->create_response(*client->get_request(), client);
								client->set_fin(true);
							}
							else
							{
								client->get_response().append(msg, receive);
								memset(msg, 0, sizeof(msg));
							}
						}
						else if ((client->is_cgi() == false || client->get_fin()) && FD_ISSET(i, &writefds_backup))
						{
							if(write_to_client(i) < 0)
							{
								std::stringstream ss;
								ss << "Error writing to client." << std::endl;
								Log::log(ss.str(), ERROR_FILE | STD_ERR);
							}
						}
					}

				}
			}
		}
		close_timed_out_clients();
	}
	return 0;
}

void Director::clear_file_descriptor(int client_fd, bool close_fd)
{
	if (FD_ISSET(client_fd, &_read_fds))
	{
		FD_CLR(client_fd, &_read_fds);
		if (client_fd == _fdmax)
		{
			_fdmax--;
		}
	}
	if (FD_ISSET(client_fd, &_write_fds))
	{
		FD_CLR(client_fd, &_write_fds);
		if (client_fd == _fdmax)
		{
			_fdmax--;
		}
	}
	if (close_fd == true)
	{
		close(client_fd);
	}
}

void Director::cgi_timeout(int client_fd, ClientInfo* client)
{
	if (client && client->get_type() == CLIENT_NODE && client->is_cgi() == true)
	{
		Log::log("Error reading CGI response on socket " +
		Utils::itoa(client_fd) + ": client timed out.\n",
		STD_ERR | ERROR_FILE);
		
		clear_file_descriptor(client->get_cgi()->response_fd[0]);
		clear_file_descriptor(client->get_cgi()->request_fd[0]);
		kill(client->get_pid(), SIGKILL);
	}
}

void	Director::close_timed_out_clients()
{
	std::vector <int> timed_out_clients = get_timed_out_clients();

	for (size_t i = 0; i < timed_out_clients.size(); i++)
	{
		ClientInfo* client = dynamic_cast<ClientInfo*>(_nodes[timed_out_clients[i]]);
		cgi_timeout(timed_out_clients[i], client);
		send_timeout_response(timed_out_clients[i], client);
		close_client_connection(timed_out_clients[i]);
	}
}

void	Director::send_timeout_response(int client_fd, ClientInfo* client)
{
	if (client)
	{
		client->get_request()->set_errcode(408);
		client->get_server()->create_response(*client->get_request(), client);
		write_to_client(client_fd);
	}
}

std::vector <int>	Director::get_timed_out_clients()
{
	std::vector <int> timed_out_clients;
	time_t current_time = time(NULL);
	int timeout = 60;

	for (std::map<int, TimeoutInfo>::iterator client = _client_timeouts.begin(); client != _client_timeouts.end(); client++)
	{
		if (this->_nodes.find(client->first) != this->_nodes.end() && client->second.client->get_type() == CLIENT_NODE && client->second.client->is_cgi() == true)
		{
			timeout = 5;
		}
		if (client->second.last_activity < current_time - timeout)
		{
			timed_out_clients.push_back(client->first);
		}
	}
	return timed_out_clients;
}

void Director::close_client_connection(int client_fd, const std::string& message)
{
	if (message.empty() == false)
	{
		Log::log(message, STD_OUT | ACCEPT_FILE);
	}

	clear_file_descriptor(client_fd);
	delete _nodes[client_fd];
	_nodes.erase(client_fd);
	_client_timeouts.erase(client_fd);
}

void	Director::close_cgi(ClientInfo* client, int status_code)
{
	if (client->get_cgi())
	{
		clear_file_descriptor(client->get_cgi()->response_fd[0]);
		clear_file_descriptor(client->get_cgi()->request_fd[0]);
		kill(client->get_pid(), SIGKILL);
		// kill is in the library: 
	}
	client->get_request()->set_errcode(status_code);
	client->get_server()->create_response(*client->get_request(), client);
								
}

// purpose: having the server socket file descriptor we create the client connection
// 			add this connection to the list of ClientInfos. Log the connection in
// 			the accept log. and set the new socket connection to non-blocking.
//
// argument: listener -> the servers socket that got the client message 
//
// return: int -> -1 if it failed and 0 for success
int	Director::create_client_connection(int listener)
{
	struct sockaddr_storage remoteaddr;
	socklen_t 				addrlen = sizeof remoteaddr;

	int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
	if (newfd == -1)
	{
		throw std::runtime_error(
			"Error: Could not accept client: " +
			std::string(strerror(errno)) +
			".\n"
		);
	}
	else
	{
		if (_nodes.find(newfd) == _nodes.end())
		{
			if (_fdmax < newfd)
			{
				_fdmax = newfd;
			}
			ClientInfo* newcl = new ClientInfo(newfd, remoteaddr, (size_t)addrlen);
			_client_timeouts[newfd].last_activity = time(NULL);
			_client_timeouts[newfd].client = newcl;
			_nodes[newfd] = newcl;
			newcl->set_server(dynamic_cast<Server*>(_nodes[listener]));
		}
		else
		{
			close_client_connection(newfd);

			throw std::runtime_error(
				"Tried to overwrite socket: " +
				Utils::itoa(newfd) +
				".\n"
			);
		}
		std::stringstream ss2;

		Utils::notify_client_connection(
			dynamic_cast<Server*>(_nodes[listener]),
			newfd,
			remoteaddr
		);

		if (fcntl(newfd, F_SETFL, O_NONBLOCK) < 0)
		{
			close_client_connection(
				newfd,
				"Error: Could not set non-blocking I/O: " +
				std::string(strerror(errno)) +
				".\n"
			);
		}
		FD_SET(newfd, &_read_fds);
	}
	return 0;
}

// purpose: we check if the client closed the connection. if he did we log it,
// 			free the ClientInfo and erase the client_fd from the select set and 
// 			the iterator list. Else we parse (TODO danil) the request message and fill
// 			the ClienInfo with information. After parsing we put the socket in 
// 			the set of writing sockets of the select, so we can answer the request. 
//
// argument: clientfd -> the file descriptor
//
// return: int -> -1 if it failed and 0 for success
int	Director::read_from_client(int client_fd)
{
	static std::map<int ,std::string>		requestmsg;
	char									remote_IP[INET6_ADDRSTRLEN];
	int										flag = 0;
	ClientInfo								*client;

	client = dynamic_cast<ClientInfo *>(_nodes[client_fd]);
	flag = Request::read_request(client_fd, MSG_SIZE, client->_read_msg);
	if (flag == 0)
	{
		close_client_connection(
			client_fd,
			"Connection closed by " +
			std::string(inet_ntop(
				AF_INET,
				get_in_addr((struct sockaddr *)&client->get_addr()),
				remote_IP,
				INET6_ADDRSTRLEN
				)) +
			" on socket " +
			Utils::itoa(client_fd) + ".\n"
		);
		client->_read_msg.clear();
		// client->get_request()->clean();
		return 0;
	}
	else if (flag == -1)
	{
		close_client_connection(client_fd, "Error: Could not read from socket: " + Utils::itoa(client_fd));
		client->get_request()->clean();
		client->_read_msg.clear();
		return -1;	
	}
	else if (flag == READ)
	{
		_client_timeouts[client_fd].last_activity = time(NULL);
		client->get_request()->init(client->_read_msg);

		Log::log(
			"Request: " +
			Utils::itoa(client_fd) +
			" parsed: " +
			client->get_request()->get_method() +
			" " + client->get_request()->get_path() + ".\n"
			, STD_OUT
		);

		std::vector<Server*> servers = _config->get_servers();
		std::vector<Server*>::iterator it;

		bool wrong_host = true;
		for (it = servers.begin(); it != servers.end(); it++)
		{
			if ((*it)->get_host_address().s_addr == client->get_server()->get_host_address().s_addr &&
			(*it)->get_port() == client->get_server()->get_port())
			{
				std::vector<std::string> host_names = (*it)->get_server_name(); 
				std::vector<std::string>::iterator host_it;
				std::string host_header = Utils::to_lower(client->get_request()->get_header("HOST"));

				for (host_it = host_names.begin(); host_it != host_names.end(); host_it++) 
				{
					if (Utils::to_lower(*host_it) == host_header)
					{
						wrong_host = false;
						client->set_server(*it);
					}
				}
			}
		}
		if (wrong_host == true && client->get_request()->get_errcode() == 0)
		{
			client->get_request()->set_errcode(404);
		}
		client->get_server()->create_response(*client->get_request(), client);
		if (client->is_cgi())
		{
			add_file_descriptor(client->get_cgi()->request_fd[1], _write_fds);
			add_file_descriptor(client->get_cgi()->response_fd[0], _read_fds);
			close(client->get_cgi()->request_fd[0]);
			close(client->get_cgi()->response_fd[1]);
		}	
		clear_file_descriptor(client_fd, false);
		add_file_descriptor(client_fd, _write_fds);
		client->_read_msg.clear();
	}
	return 0;
}

int	Director::write_to_client(int fd)
{
	int				num_bytes;
	ClientInfo*		client = dynamic_cast<ClientInfo*>(_nodes[fd]);

	std::string content = client->get_response();
	int size = content.size();

	if (size < MSG_SIZE)
	{
		num_bytes = write(fd, content.c_str(), size);
	}
	else
	{
		num_bytes = write(fd, content.c_str(), MSG_SIZE);
	}
	if (num_bytes < 0)
	{
		close_client_connection(
			fd,
			"Error sending a response: " +
			std::string(strerror(errno)) +
			"\n."
		);
	}
	else if (num_bytes == (int)(content.size()) || num_bytes == 0)
	{
		Log::log(
			"Response " +
			client->get_request()->get_path() +
			" sent to socket" +
			Utils::itoa(fd) + ".\n",
			STD_OUT
		);

		if(	client->get_request()->get_errcode() != 0 || client->is_cgi() == true)
		{
			close_client_connection(
				fd, 
				"Closing client connection on: " + 
				Utils::itoa(fd) + "\n"
			);
		}
		else
		{
			clear_file_descriptor(fd, false);
			add_file_descriptor(fd, _read_fds);
			client->get_request()->clean();
			client->clear_response();
		}
	}
	else
	{
		client->set_response(client->get_response().substr(num_bytes));
	}
	return (0);
}