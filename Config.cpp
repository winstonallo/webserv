/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sgiochal <sgiochal@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 18:45:36 by sgiochal         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"

Config::Config()
{
	load_config("default_config.conf");
}

Config::~Config()
{

}

Config::Config(const Config& rhs)
{
	*this = rhs;
}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
	{
		servconf = rhs.servconf;
	}
	return (*this);
}

Config::Config(const std::string &path)
{
	load_config(path);
}

void Config::load_config(const std::string &path)
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
