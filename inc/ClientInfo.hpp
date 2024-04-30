#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctime>
#include "Node.hpp"
#include "Request.hpp"
#include "Server.hpp"
#include "CGI.hpp"

class Server;

class ClientInfo : public Node 
{
	public:
									~ClientInfo();
		void						clean();

									ClientInfo() :  _cgi(NULL), _isCGI(false) {}
									ClientInfo(int fd, const struct sockaddr_storage& addr, size_t addr_len);

		CGI*						get_cgi() { return _cgi; }
		void						set_cgi(CGI* cgi) { _cgi = cgi; }
		bool						is_cgi() const { return _isCGI; }
		void						set_is_cgi(bool isCGI) { _isCGI = isCGI; }
		int							get_pid() const { return _pid; }
		void						set_pid(int pid) { _pid = pid; }
		bool						get_fin() const { return _fin; }
		void						set_fin(bool fin) { _fin = fin; }
		Server*						get_server() const { return _server; }
		void						set_server(Server* server) { _server = server; }
		Request*					get_request() { return &_request; }
		std::string&				get_response() { return _response; }
		void						set_response(const std::string& response) { _response = response; }
		void						clear_response() { _response.clear(); }
		void						clear_request() { _request.clean(); }

		std::string					_response;
		std::string					_read_msg;

	private:

		Server*						_server;
		Request						_request;
		CGI*						_cgi;
		bool						_isCGI;
		int							_pid;
		bool						_fin;

		ClientInfo&					operator=(const ClientInfo&) { return *this; }
									ClientInfo(const ClientInfo&) {}
};

#endif
