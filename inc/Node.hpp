/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/10 16:29:24 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/11 12:17:27 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

enum node_types 
{
	UNKNOWN_NODE,
	SERVER_NODE,
	CLIENT_NODE
};

class Node 
{
	public:
									Node();
									~Node();
									Node(int fd, 
										const struct sockaddr_storage& addr,
										size_t addr_len,
										enum node_types tp);
									Node(const Node& rhs);
		Node&						operator=(const Node& rhs);


		int							get_fd() const;
		void						set_fd(int tfd);
		node_types					get_type() const;
		void						set_type(enum node_types t);
		struct sockaddr_storage		get_addr() const;
		void						set_addr(const struct sockaddr_storage& val);
		size_t						get_addr_len() const;
		void						set_addr_len(size_t len);
		static void*				get_in_addr(struct sockaddr *sa);

	protected:
		int							fd;
		struct sockaddr_storage		addr;
		size_t						addr_len;
		enum node_types				type;
};

std::ostream& operator<<(std::ostream& os, const Node& nd);

#endif
