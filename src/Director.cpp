#include "Director.hpp"
#include <cstddef>
#include <ctime>
#include <netdb.h>
#include <sstream>
#include <string.h>
#include <string>
#include <map>
#include <sys/select.h>
#include <sys/wait.h>
#include <vector>
#include "ClientInfo.hpp"
#include "Log.hpp"
#include "Node.hpp"
#include "Server.hpp"
#include "Utils.hpp"

Director::Director(const std::string& config_path):fdmax(-1)
{
	config = new Config(config_path);
}

Director::~Director()
{
	std::map <int, Node*>::iterator it;
	for (it = _nodes.begin(); it != _nodes.end(); it++)
	{
		if (it->second->get_type() == CLIENT_NODE)
		{
			ClientInfo *ci = static_cast<ClientInfo *>(it->second);
			delete ci;
		}
	}
	delete config;
}

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

Config*	Director::get_config()
{
	return config;
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
		return &(((struct sockaddr_in *)sa)->sin_addr);
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
int	Director::init_server(Server *si)
{
	si->set_director(this);
	struct addrinfo hints, *ai, *p;
	int listener;
	int rv, yes=1 ;
	
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
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
		si->set_addr_len((size_t)p->ai_addrlen);

		Log::log("Server created on localhost with domain name: " +
		si->get_server_name()[0] + ", port: " + Utils::itoa(si->get_port()) + "\n",
		ACCEPT_FILE | STD_OUT);

		break;
	}

	if (p == NULL)
	{
		std::stringstream ss;
		ss << "Select server failed to bind." << std::endl;
		Log::log(ss.str(), ERROR_FILE | STD_ERR);
		freeaddrinfo(ai);
		return -1;
	}
									fdmax--;
	si->set_fd(listener);
	freeaddrinfo(ai);
	return 0;
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
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	std::vector<Server*> 			servers = config->get_servers();
	std::vector<Server*>::iterator 	e = servers.end();
	std::vector<Server*>::iterator 	it ;
	std::vector<Server*>::iterator 	sub_it;
	bool							same_socket;	

	// take virtual servers into account
	for (it = servers.begin(); it != e; it++)
	{
		same_socket = false;
		for (sub_it = servers.begin(); sub_it != it; sub_it++) 
		{
			if ((*sub_it)->get_host_address().s_addr == (*it)->get_host_address().s_addr &&
				(*sub_it)->get_port() == (*it)->get_port())
			{
				(*it)->set_fd((*sub_it)->get_fd());
				same_socket = true;
			}
		}
		if (same_socket == false)
		{
			if (init_server(*it) < 0)
				return -1;
		}
	}

	//make servers non-blocking and listen
	for (it = servers.begin(); it != e; it++)
	{
		int listener = (*it)->get_fd();
		if (fcntl(listener, F_SETFL, O_NONBLOCK) < 0)
		{
			std::stringstream ss;
			ss << "Error unblocking socket: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		if (listen(listener, 512) == -1)
		{
			std::stringstream ss;
			ss << "Error listening: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		FD_SET(listener, &read_fds);
		if (fdmax < listener) fdmax = listener;
		_nodes[listener] = *it;
		_nodes[listener]->set_type(SERVER_NODE);
		_nodes[listener]->set_fd(listener);
	}
	return 0;
}

extern bool is_running;
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
	ClientInfo* 				cl;


	while (is_running)
	{
		readfds_backup = read_fds;
		writefds_backup = write_fds;
		timeout_time.tv_sec = 1;
		timeout_time.tv_usec = 0;
		std::vector <int> to_delete;
		for (size_t i = 0; i < to_delete.size(); i++)
		{
			_client_timeouts.erase(to_delete[i]);
		}
		to_delete.clear();
		if ((ret = select(fdmax + 1, &readfds_backup, &writefds_backup, NULL, &timeout_time)) < 0 )
		{
			std::stringstream ss;
			if (is_running == false)
				break;
			ss << "Error while select: " << strerror(errno) << std::endl;
			Log::log(ss.str(), ERROR_FILE | STD_ERR);
			return -1;
		}
		for (int i = 0; i <= fdmax; i++)
		{
			if (_nodes.find(i) != _nodes.end())
			{
				// FOR SERVERS
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
							Log::log("Error creating client connection: " + std::string(e.what()) + "\n", STD_ERR | ERROR_FILE);
						}
					}
				}
				// FOR CLIENTS
				else if (_nodes[i] != NULL and _nodes[i]->get_type() == CLIENT_NODE)
				{
					cl = static_cast<ClientInfo*>(_nodes[i]);
					if (FD_ISSET(i, &readfds_backup))
					{
						try
						{
							if (read_from_client(i) < 0)
							{
								std::stringstream ss;
								ss << "Error reading from client: " << std::endl;
								Log::log(ss.str(), ERROR_FILE | STD_ERR);
							}
						}
						catch(const std::exception& e)
						{
							Log::log("Error reading from client: " + std::string(e.what()) + "\n", STD_ERR | ERROR_FILE);
						}
					}
					// WRITING
					if (FD_ISSET(i, &writefds_backup))
					{
						// give request body to CGI
						if (cl->is_cgi() && FD_ISSET(cl->get_cgi()->request_fd[1], &writefds_backup) && !cl->get_fin())
						{
							int send;

							std::string&	reqb = cl->get_request()->get_body();
							if (!reqb.size())
								send = 0;
							else if (reqb.size() >= MSG_SIZE)
								send = write(cl->get_cgi()->request_fd[1], reqb.c_str(), MSG_SIZE);
							else
								send = write(cl->get_cgi()->request_fd[1], reqb.c_str(), reqb.size());
							if (send < 0)
							{
								std::stringstream ss;
								ss << "Error sending request body to CGI: " << strerror(errno);
								Log::log(ss.str(), STD_ERR | ERROR_FILE);
								close_cgi(cl, 500);
							}
							else if (send == 0 || (size_t) send == reqb.size())
							{
								clear_file_descriptor(cl->get_cgi()->response_fd[1]);
								clear_file_descriptor(cl->get_cgi()->request_fd[1]);
							}
							else
							{
								cl->set_time();
								reqb = reqb.substr(send);
							}
						}
						// cgi tiemout
						else if (cl->get_cgi() && FD_ISSET(cl->get_cgi()->response_fd[0], &readfds_backup) && !cl->get_fin())
						{
							char	msg[MSG_SIZE * 4];
							int		receive = 0;
							int		status = 0;

							receive = read(cl->get_cgi()->response_fd[0], msg, MSG_SIZE * 4);
							if (receive == 0)
							{
								clear_file_descriptor(cl->get_cgi()->response_fd[0]);
								waitpid(cl->get_pid(), &status, WNOHANG);
								if (WEXITSTATUS(status) != 0)
								{
									cl->get_request()->set_errcode(500);
									cl->get_server()->create_response(*cl->get_request(), cl);
								}
								if (cl->get_response().find("HTTP/1.1") == std::string::npos)
									cl->get_response().insert(0, "HTTP/1.1 200 OK\r\n");
								cl->set_fin(true);
							}
							else if (receive < 0)
							{
								std::stringstream ss;
								ss << "Error reading CGI response: " << strerror(errno);
								Log::log(ss.str(), STD_ERR | ERROR_FILE);
								clear_file_descriptor(cl->get_cgi()->response_fd[0]);
								clear_file_descriptor(cl->get_cgi()->request_fd[0]);
								cl->get_request()->set_errcode(500);
								cl->get_server()->create_response(*cl->get_request(), cl);
								cl->set_fin(true);
							}
							else
							{
								cl->get_response().append(msg, receive);
								memset(msg, 0, sizeof(msg));
							}
						}
						else if ((cl->is_cgi() == false || cl->get_fin()) && FD_ISSET(i, &writefds_backup))
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
	if (FD_ISSET(client_fd, &read_fds))
	{
		FD_CLR(client_fd, &read_fds);
		if (client_fd == fdmax)
		{
			fdmax--;
		}
	}
	if (FD_ISSET(client_fd, &write_fds))
	{
		FD_CLR(client_fd, &write_fds);
		if (client_fd == fdmax)
		{
			fdmax--;
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
	int timeout_seconds = 60;

	for (std::map<int, TimeoutInfo>::iterator client = _client_timeouts.begin(); client != _client_timeouts.end(); client++)
	{
		if (this->_nodes.find(client->first) != this->_nodes.end() && client->second.client->get_type() == CLIENT_NODE && client->second.client->is_cgi() == true)
		{
			timeout_seconds = 5;
		}
		if (client->second.last_activity < current_time - timeout_seconds)
		{
			timed_out_clients.push_back(client->first);
		}
	}
	return timed_out_clients;
}

void Director::close_client_connection(int client_fd)
{
	clear_file_descriptor(client_fd, false);
	clear_file_descriptor(client_fd, false);
	_client_timeouts.erase(client_fd);
	_nodes.erase(client_fd);
	delete _nodes[client_fd];
	close(client_fd);
}

// purpose: close the cgi client and log the status code
//
// argument: client -> the client that is a cgi
// 			 status_code -> the status code of the response
void	Director::close_cgi(ClientInfo* client, int status_code)
{
	if (client->get_cgi())
	{
		clear_file_descriptor(client->get_cgi()->response_fd[0]);
		clear_file_descriptor(client->get_cgi()->request_fd[0]);
		kill(client->get_pid(), SIGKILL);
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
		if (_nodes.find(newfd) == _nodes.end())
		{
			ClientInfo *newcl = new ClientInfo(newfd, remoteaddr, (size_t)addrlen);
			_client_timeouts[newfd].last_activity = time(NULL);
			_client_timeouts[newfd].client = newcl;
			newcl->set_server(dynamic_cast<Server*>(_nodes[listener]));
			_nodes[newfd] = newcl;
		}
		else
		{
			std::stringstream ss;
			ss << "Tried to overwrite socket: " << newfd << std::endl;
			Log::log(ss.str(), STD_ERR | ERROR_FILE);
			_nodes.erase(newfd);
			_client_timeouts.erase(newfd);
			close(newfd);
			throw std::runtime_error("");
		}
		std::stringstream ss2;

		ss2 << "New connection from ";
		ss2 << inet_ntop(remoteaddr.ss_family,
						get_in_addr((struct sockaddr *)&remoteaddr),
						remoteIP, INET6_ADDRSTRLEN);
		ss2 << " on socket " << newfd << std::endl;
		Utils::notify_client_connection(dynamic_cast<Server*>(_nodes[listener]), newfd, remoteaddr);
		if (fcntl(newfd, F_SETFL, O_NONBLOCK) < 0)
		{
			std::stringstream ss3;
			ss3 << "Error while non-blocking: " << strerror(errno) << std::endl;
			Log::log(ss3.str(), ERROR_FILE | STD_ERR);
			delete _nodes[newfd];
			_nodes.erase(newfd);
			close(newfd);
		}
		FD_SET(newfd, &read_fds);
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
	char									remoteIP[INET6_ADDRSTRLEN];
	int										flag = 0;
	ClientInfo								*ci;

	ci = dynamic_cast<ClientInfo *>(_nodes[client_fd]);
	flag = Request::read_request(client_fd, MSG_SIZE, ci->_read_msg);
	if (!flag)
	{
		std::stringstream ss;
		ss << "Connection closed by " << inet_ntop(AF_INET, get_in_addr((struct sockaddr *)&ci->get_addr()),
						remoteIP, INET6_ADDRSTRLEN);
		ss << " on socket " << client_fd << std::endl;
		Log::log(ss.str(), ACCEPT_FILE | STD_OUT);
		clear_file_descriptor(client_fd);
		ci->get_request()->clean();
		ci->_read_msg.clear();
		delete _nodes[client_fd];
		_nodes.erase(client_fd);
		return 0;
	}
	else if (flag == -1)
	{
		clear_file_descriptor(client_fd);
		ci->get_request()->clean();
		_nodes.erase(client_fd);
		delete _nodes[client_fd];
		std::stringstream ss;
		ss << "Error reading from socket: " << client_fd << std::endl;
		Log::log(ss.str(), ERROR_FILE | STD_ERR);
		ci->_read_msg.clear();
		return -1;	
	}
	else if (flag == READ)
	{
		_client_timeouts[client_fd].last_activity = time(NULL);
		try
		{
			ci->get_request()->init(ci->_read_msg);
		}
		catch(const std::exception& e)
		{}
		// print Request parsed log
		std::stringstream ss;
		ss << "Request: " << client_fd << " parsed: " << ci->get_request()->get_method();
		ss << " " << ci->get_request()->get_path() << std::endl;
		Log::log(ss.str(), STD_OUT);

		// virtual servers, we go throug the servers and match the host name / server name 
		std::vector<Server*> servers = config->get_servers();
		std::vector<Server*>::iterator it;
		for (it = servers.begin(); it != servers.end(); it++)
		{
			if ((*it)->get_host_address().s_addr == ci->get_server()->get_host_address().s_addr &&
			(*it)->get_port() == ci->get_server()->get_port())
			{
				std::vector<std::string> host_names = (*it)->get_server_name(); 
				std::vector<std::string>::iterator host_it;
				std::string host_header = Utils::to_lower(ci->get_request()->get_header("HOST"));
				for (host_it = host_names.begin(); host_it != host_names.end(); host_it++) 
				{
					if (Utils::to_lower(*host_it) == host_header)	
						ci->set_server(*it);
				}
			}
		}
		ci->get_server()->create_response(*ci->get_request(), ci);
		if (ci->is_cgi())
		{
			FD_SET(ci->get_cgi()->request_fd[1], &write_fds);
			if (ci->get_cgi()->request_fd[1] > fdmax)
				fdmax = ci->get_cgi()->request_fd[1];
			FD_SET(ci->get_cgi()->response_fd[0], &read_fds);
			if (ci->get_cgi()->response_fd[0] > fdmax)
				fdmax = ci->get_cgi()->response_fd[0];
		}	
		FD_CLR(client_fd, &read_fds);
		if (client_fd == fdmax)	fdmax--;
		FD_SET(client_fd, &write_fds);
		if (client_fd > fdmax)	fdmax = client_fd;
		// ci->get_request()->clean();
		ci->_read_msg.clear();
	}
	ci->set_time();
	return 0;
}

// purpose: (TODO) after getting the request message, this function sends back
// 			the answer, which in a simple case is the requested file or in case of a
// 			cgi the generated file. 
//
// return: int -> -1 if it failed and 0 for success
int	Director::write_to_client(int fd)
{
	int				num_bytes;
	ClientInfo*		cl = dynamic_cast<ClientInfo*>(_nodes[fd]);

	std::string content = cl->get_response();
	int sz = content.size();

	if (sz < MSG_SIZE)
		num_bytes = write(fd, content.c_str(), sz);
	else
		num_bytes = write(fd, content.c_str(), MSG_SIZE);
	if (num_bytes < 0)
	{
		std::stringstream ss;
		ss << "Error sending a response: " << strerror(errno) << std::endl;
		Log::log(ss.str(), STD_ERR | ERROR_FILE);
		clear_file_descriptor(fd);
		_nodes.erase(fd);
		delete _nodes[fd];
	}
	else if (num_bytes == (int)(content.size()) || num_bytes == 0)
	{
		std::stringstream ss;
		ss << "Response " << cl->get_request()->get_path() << " send to socket:" << fd << std::endl;
		Log::log(ss.str(), STD_OUT);
		if(	cl->get_request()->get_errcode() || cl->is_cgi())
		{
			std::stringstream ss;
			ss << "Closing client connection on: " << fd << std::endl;
			Log::log(ss.str(), STD_OUT );
			clear_file_descriptor(fd);
			delete _nodes[fd];
			_nodes.erase(fd);
		}
		else
		{
			FD_CLR(fd, &write_fds);
			if (fd == fdmax) { fdmax--; }
			FD_SET(fd, &read_fds);
			if (fd > fdmax) { fdmax=fd; } 
			cl->get_request()->clean();
			cl->clear_response();
		}
	}
	else
	{
		cl->set_time();
		cl->set_response(cl->get_response().substr(num_bytes));
	}
	return (0);
}