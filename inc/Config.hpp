/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abied-ch <abied-ch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 19:57:02 by abied-ch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

class Config 
{
	public:
		Config(const std::string& path = "default_config.conf");
		~Config();
		
		void							load_config(const std::string& path);

	private:
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
		int								port;
		std::vector<std::string>		server_name;
		std::string						access_logs_path;
};

#endif