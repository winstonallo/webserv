#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctime>
#include "Node.hpp"
#include "Request.hpp"
#include "ServerInfo.hpp"

//purpose:	data filled from the request parsing for and used for 
//			answering requests

class ServerInfo;

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
		void						set_time();
		ServerInfo*					get_server_info() const;
		void						set_server_info(ServerInfo* si);
		// void						set_request(Request rq);
		// Request						get_request() const;

	private:
		ServerInfo*					server_info;
		time_t						prev_time;
		//Request					request;

};

#endif
