#include "ClientInfo.hpp"

ClientInfo::ClientInfo():_cgi(CGI())
{
	_isCGI = false;
}

ClientInfo::~ClientInfo()
{

}

ClientInfo::ClientInfo(int tfd, const struct sockaddr_storage& address, size_t taddrlen) :
	Node(tfd, address, taddrlen, CLIENT_NODE), _cgi(CGI())
{
	type = CLIENT_NODE;
	_isCGI = false;
	_prev_time = time(NULL);
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

CGI&	ClientInfo::get_cgi()
{
	return _cgi;
}

time_t	ClientInfo::get_prev_time() const
{
	return _prev_time;
}

bool	ClientInfo::is_cgi() const
{
	return _isCGI;
}

void	ClientInfo::set_is_cgi(bool b)
{
	_isCGI = b;
}

void	ClientInfo::set_prev_time(time_t tm)
{
	_prev_time = tm;
}

void	ClientInfo::set_time()
{
	_prev_time = time(NULL);
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

std::string	ClientInfo::get_response() const
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


// void	ClientInfo::set_request(Request rq)
// {
// 	request = rq;
// }

// Request	ClientInfo::get_request() const
// {
// 	return request;
// }