#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include "Node.hpp"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

//purpose:	data filled from the request parsing for and used for 
//			answering requests

class ClientInfo : public Node 
{
	public:
									ClientInfo();
									~ClientInfo();
									ClientInfo(const ClientInfo& rhs);
									ClientInfo(int fd, const struct sockaddr_storage& addr, size_t addr_len);
		ClientInfo&					operator=(const ClientInfo& rhs);
	private:
};

#endif
