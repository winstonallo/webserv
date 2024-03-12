/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 10:35:13 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/11 07:30:52 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTINFO_HPP
#define CLIENTINFO_HPP

#include <iostream>
#include "Node.hpp"

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
