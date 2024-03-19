#ifndef SERVER_HPP
#define SERVER_HPP

#include "Request.hpp"
#include "ServerInfo.hpp"

class Request;

class Server
{
	public:
					Server();
					~Server();
		Server&		operator=(const Server& rhs);
					Server(const Server& rhs);
		int			respond(Request rq);

		ServerInfo	get_server_info() const;
		Request		get_request() const;

	private:
		Request						request;
		ServerInfo					server_info;
		std::map<int, ClientInfo>	clients;

};

#endif