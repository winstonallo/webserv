#include "ClientInfo.hpp"

ClientInfo::~ClientInfo()
{
	if (_cgi)
		delete _cgi;
}

ClientInfo::ClientInfo(
	int tfd,
	const struct sockaddr_storage& address,
	size_t taddrlen
	) :
	Node(
		tfd,
		address,
		taddrlen,
		CLIENT_NODE
		)
{
	_cgi = NULL;
	type = CLIENT_NODE;
	_isCGI = false;
	_pid = -1;
	_fin = false;
}

void	ClientInfo::clean()
{
	_response.clear();
	if (_cgi)
		delete _cgi;
	_isCGI = false;
	_pid = -1;
	_fin = false;
}