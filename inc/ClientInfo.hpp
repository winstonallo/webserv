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

		ClientInfo() :
		_cgi(NULL),
		_isCGI(false) {}

		ClientInfo(
			int fd,
			const struct sockaddr_storage& address,
			size_t addr_len
		) : Node(
				fd,
				address,
				addr_len,
				CLIENT_NODE
			),
			_cgi(NULL),
			_isCGI(false),
			_pid(-1),
			_fin(false) {}

		~ClientInfo()
		{
			if (_cgi != NULL)
			{
				delete _cgi;
			}
		}
		
		void clean()
		{
			_response.clear();
			if (_cgi != NULL)
			{
				delete _cgi;
			}
			_isCGI = false;
			_pid = -1;
			_fin = false;
		}


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

		std::string&				get_response() { return _response; }
		void						set_response(const std::string& response) { _response = response; }

		void						clear_response() { _response.clear(); }
		void						clear_request() { _request.clean(); }
		Request*					get_request() { return &_request; }

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
