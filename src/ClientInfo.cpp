#include "ClientInfo.hpp"

ClientInfo::ClientInfo()
{
	_isCGI = false;
	_cgi = NULL;
}

ClientInfo::~ClientInfo()
{
	if (_cgi)
		delete _cgi;
}

ClientInfo::ClientInfo(int tfd, const struct sockaddr_storage& address, size_t taddrlen) :
	Node(tfd, address, taddrlen, CLIENT_NODE), _cgi(NULL)
{
	type = CLIENT_NODE;
	_isCGI = false;
	_pid = -1;
	_fin = false;
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

CGI*	ClientInfo::get_cgi()
{
	return _cgi;
}

void	ClientInfo::set_cgi(CGI *cg)
{
	_cgi = cg; 
}

bool	ClientInfo::is_cgi() const
{
	return _isCGI;
}

void	ClientInfo::set_is_cgi(bool b)
{
	_isCGI = b;
}

void	ClientInfo::set_server(Server *si)
{
	_server = si;
}

Server*		ClientInfo::get_server() const
{
	return _server;
}

Request*	ClientInfo::get_request()
{
	return &_request;
}

std::string&	ClientInfo::get_response()
{
	return _response;
}

void	ClientInfo::set_response(const std::string& res)
{
	_response = res;
}

void	ClientInfo::clear_response()
{
	_response.clear();
}

void	ClientInfo::clear_request()
{
	_request.clean();
}

int		ClientInfo::get_pid() const
{
	return _pid;
}

void	ClientInfo::set_pid(int p)
{
	_pid = p;
}

bool	ClientInfo::get_fin() const
{
	return _fin;
}

void	ClientInfo::set_fin(bool f)
{
	_fin = f;
}
// void	ClientInfo::set_request(Request rq)
// {
// 	request = rq;
// }

// Request	ClientInfo::get_request() const
// {
// 	return request;
// }

void	ClientInfo::clean()
{
	_response.clear();
	if (_cgi)
		delete _cgi;
	_isCGI = false;
	_pid = -1;
	_fin = false;
}