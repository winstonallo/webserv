/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Director.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 12:17:46 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/09 00:47:40 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DIRECTOR_HPP
#define DIRECTOR_HPP

#include <iostream>
#include <string.h>
#include <map>
#include <vector>
#include "Node.hpp"
#include "ClientInfo.hpp"
#include "ServerInfo.hpp"
#include "Log.hpp"

#define MSG_SIZE 50000

class Director 
{
	public:
										Director();
										~Director();
		void							add_server_info(ServerInfo si);
		int								init_servers();
		int								run_servers();

	private:
										Director(const Director& rhs);
		Director&						operator=(const Director& rhs);
		void							*get_in_addr(struct sockaddr *sa);
		int								init_server(ServerInfo *si);
		int								create_client_connection(int listener);
		int								read_from_client(int fd);
		int								write_to_client(int fd);

		std::vector<ServerInfo>			server_infos;
		std::map<int, Node*>			nodes;
		fd_set							read_fds, write_fds;
		int								fdmax;
};

#endif
