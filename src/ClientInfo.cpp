/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 10:35:13 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/11 18:11:10 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


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

ClientInfo::ClientInfo(const ClientInfo& rhs)
{
	*this = rhs;
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



