#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include "Headers.hpp"

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
