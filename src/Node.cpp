/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 16:29:24 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/12 19:30:36 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Node.hpp"

Node::Node()
{

}

Node::~Node()
{

}

Node::Node(int tfd, const struct sockaddr_storage& taddr, size_t taddr_len, enum node_types tp):
	fd(tfd), addr(taddr), addr_len(taddr_len), type(tp)
{}

Node::Node(const Node& rhs)
{
	*this = rhs;
}

Node&	Node::operator=(const Node& rhs)
{
	if (this != &rhs)
	{
		fd = rhs.fd;
	}
	return (*this);
}

int	Node::get_fd() const
{
	return fd;
}

void Node::set_fd(int tfd)
{
	fd = tfd;
} 

enum node_types	Node::get_type() const
{
	return type;
}

void	Node::set_type(enum node_types tp)
{
	type = tp;
}

struct sockaddr_storage	Node::get_addr() const
{
	return (addr);
}

void	Node::set_addr(const struct sockaddr_storage& val)
{
	 addr = val;
}

void* Node::get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in *)sa)->sin_addr);
	return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

size_t	Node::get_addr_len() const
{
	return addr_len;
}

void	Node::set_addr_len(size_t sz)
{
	addr_len = sz; 
}

std::ostream& operator<<(std::ostream& os, const Node& nd)
{
	char remoteIP[INET6_ADDRSTRLEN];	
	sockaddr_storage temp = nd.get_addr();

	os << ((nd.get_type() == CLIENT_NODE) ? "Client: " : "Server: ");
	os << inet_ntop(nd.get_addr().ss_family,
					Node::get_in_addr((struct sockaddr *)&temp),
					remoteIP,
					INET6_ADDRSTRLEN);
	os << " socket: " << nd.get_fd() << std::endl;
	return (os);
}