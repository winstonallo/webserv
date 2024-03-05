/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arthur <arthur@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/04 22:49:16 by arthur           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Config.hpp"

Config::~Config() {}

Config::Config(const Config& rhs) {*this = rhs;}

Config&	Config::operator=(const Config& rhs)
{
	if (this == &rhs)
		return *this;
	this->config = rhs.config;
	return *this;
}

// default coonctructor (takes default path if called with no argument)
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
		if (line.empty() == false)
			map_line(Parser::trim(line, " ;{}\n\t"));
	}
	config_file.close();
	store_values();
}

// splits string into key-value pair & stores in config map
// returns if value is empty
// @param line: line to trim and store
void	Config::map_line(const std::string& line)
{
	std::string 		key, value;
	std::istringstream 	input(line);
	
	if (std::getline(input, key, ' '))
	{
		std::getline(input, value);
		value = Parser::trim(value, " \t\n{};");
		if (value.empty() == false)
			config[Parser::trim(key, " ;{}\n\t")] = value;
	}
}

// stores the parsed values in their respective vector
void	Config::store_values()
{
	server_names = Parser::split(config["server_name"], " ");
	allowed_methods = Parser::split(config["allow_methods"], " ");
}

// returns vector containing the server names specified in the config
std::vector <std::string> Config::get_server_names() const
{
	return server_names;
}

// returns vector containing allowed methods specified in the config
std::vector <std::string> Config::get_allowed_methods() const
{
	return allowed_methods;
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