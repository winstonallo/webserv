/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:44:26 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "ServerConfig.hpp"

class Config 
{
	public:
									Config();
									Config(const std::string& path);
		void						load_config(const std::string& path);
									~Config();

	private:
									Config(const Config& rhs);
		Config&						operator=(const Config& rhs);
		std::vector<ServerConfig>	servconf;
};

#endif
