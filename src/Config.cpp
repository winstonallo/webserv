/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abied-ch <abied-ch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 19:58:01 by abied-ch         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Config.hpp"

Config::~Config()
{

}

Config::Config(const Config& rhs)
{
	*this = rhs;
}

Config&	Config::operator=(const Config& rhs)
{
	this->access_logs_path = rhs.access_logs_path;
	this->port = rhs.port;
	this->server_name = rhs.server_name;
}

Config::Config(const std::string& path)
{
	load_config(path);
}

void Config::load_config(const std::string& path)
{
	std::string config;
	std::ifstream config_file(path.c_str());
	if (config_file.is_open() == true)
	{
		std::ostringstream out_string;
		out_string << config_file.rdbuf();
		config = out_string.str();
	}
	std::cout << config;
}
