/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: abied-ch <abied-ch@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 22:24:41 by abied-ch         ###   ########.fr       */
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
	if (this == &rhs)
		return *this;
	this->config = rhs.config;
	return *this;
}

Config::Config(const std::string& path)
{
	load_config(path);
}

// reads & stores config
// @param config_path: path to config file
void	Config::load_config(const std::string &config_path)
{
	std::string		line;
	std::ifstream	config_file(config_path.c_str());
	
	if (config_file.is_open() == false)
		throw std::runtime_error("could not open config");
	while (std::getline(config_file, line))
	{
		line = Parser::trim(line);
		if (line.empty())
			continue ;
		line_to_map(line);
	}
	config_file.close();
}

// splits string into key-value pair & stores in config map
// returns if value is empty
// @param line: line to trim and store
void	Config::line_to_map(const std::string& line)
{
	std::istringstream input(line);
	std::string key, value;
	if (std::getline(input, key, ' '))
	{
		std::getline(input, value);
		std::string trimmed_value = Parser::trim(value);
		if (trimmed_value.empty())
			return ;
		config[Parser::trim(key)] = Parser::trim(value);
	}
}

// returns value from the config map
// @param key: key to the value we want to get
std::string	Config::get_value(const std::string& key)
{
	return config[key];
}

// returns the config map
std::map <std::string, std::string>	Config::get_config() const
{
	return config;
}