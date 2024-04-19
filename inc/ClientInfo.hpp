#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctime>
#include "Node.hpp"
#include "Request.hpp"
#include "Server.hpp"

//purpose:	data filled from the request parsing for and used for 
//			answering requests

class Server;

class ClientInfo : public Node 
{
	public:
									ClientInfo();
									~ClientInfo();
									ClientInfo(const ClientInfo& rhs);
									ClientInfo(int fd, const struct sockaddr_storage& addr, size_t addr_len);
		ClientInfo&					operator=(const ClientInfo& rhs);

		time_t						get_prev_time() const;
		void						set_prev_time(time_t tm);
		CGI&						get_cgi();
		void						set_cgi(CGI* cg);
		bool						is_cgi() const;
		void						set_is_cgi(bool);
		void						set_time();
		Server*						get_server() const;
		void						set_server(Server* si);
		Request*					get_request();
		std::string					get_response() const;
		void						set_response(const std::string& res);
		void						clear_response();
		void						clear_request(); 

	private:
		Server*						_server;
		Request						_request;
		std::string					_response;
		time_t						_prev_time;
		CGI							_cgi;
		bool						_isCGI;
};

#endif
