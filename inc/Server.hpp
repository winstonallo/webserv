#ifndef SERVER_HPP
#define SERVER_HPP

#include <sstream>
#include "Request.hpp"
#include "ServerInfo.hpp"
#include "Utils.hpp"

class Request;

class Server
{
	public:
											Server();
											~Server();
		Server&								operator=(const Server& rhs);
											Server(const Server& rhs);
		int									respond(Request rq);

		ServerInfo							get_server_info() const;
		Request								get_request() const;
		int									get_error_code() const;
		void								set_error_code(int err);

	private:
		void								create_response();
		void								init_status_strings();
		void								init_content_types();

		std::map<int, std::string>			status_string;
		std::map<std::string, std::string>	content_type;

		Request								request;
		ServerInfo							server_info;
		std::map<int, ClientInfo>			clients;
		std::string							response;
		int									errcode;
};

#endiF