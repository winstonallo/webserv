#include "ClientInfo.hpp"

ClientInfo::ClientInfo()
{

}

ClientInfo::~ClientInfo()
{

}

ClientInfo::ClientInfo(int tfd, const struct sockaddr_storage& address, size_t taddrlen) :
	Node(tfd, address, taddrlen, CLIENT_NODE)
{
	type = CLIENT_NODE;
}

ClientInfo::ClientInfo(const ClientInfo& rhs) : Node()
{
	(void)rhs;
}

ClientInfo&	ClientInfo::operator=(const ClientInfo& rhs)
{
	if (this != &rhs)
	{
		fd = rhs.fd;
		addr = rhs.addr;
	}
	return (*this);
}



