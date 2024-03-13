#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include "Node.hpp"

class ClientInfo : public Node 
{
	public:
									ClientInfo();
									~ClientInfo();
									ClientInfo(int fd, const struct sockaddr_storage& addr, size_t addr_len);
		ClientInfo&					operator=(const ClientInfo& rhs);
									ClientInfo(const ClientInfo& rhs);
	private:
};

#endif
