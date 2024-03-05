/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/04 17:57:16 by sgiochal          #+#    #+#             */
/*   Updated: 2024/03/05 13:23:32 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/Config.hpp"

Config::~Config() {}

Config::Config(const Config& rhs) {*this = rhs;}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
	{
		config = rhs.config;
	}
	return (*this);
}

// default constructor (uses default path if called with no argument)
Config::Config(const std::string& path)
{
	load_config(path);
}

// reads & stores config
// @param config_path: path to config file
void Config::load_config(const std::string &config_path) 
{
    std::string line;
    std::ifstream config_file(config_path.c_str());
    std::string current_primary_key;
	std::string key;
    
    if (!config_file.is_open())
        throw std::runtime_error("could not open config file");

    while (std::getline(config_file, line))
	{
        line = Parser::trim(line, " \t\n");
        if (line.empty())
            continue;

        if (line[line.size() - 1] == '{')
		{
            current_primary_key = Parser::trim(key.substr(0, key.size()), " \t\n");
		}
        else if (line.find('}') != std::string::npos || current_primary_key == "server")
            current_primary_key.clear();
        else 
		{
            std::vector<std::string> temp = Parser::split(line, " \t\n;{}");
            if (current_primary_key.empty() == false)
			{
                for (size_t i = 1; i < temp.size(); i++)
                    config[current_primary_key][temp[0]].push_back(temp[i]);
            } 
			else 
			{
                for (size_t i = 1; i < temp.size(); i++)
                    config[TOP_LEVEL][temp[0]].push_back(temp[i]);
            }
        }
		key = line;
    }
    config_file.close();
}

// splits string into key-value pair & stores in config map
// returns if value is empty
// @param line: line to trim and store

// stores the parsed values in their respective vector

// returns vector containing the server names specified in the config

// returns vector containing allowed methods specified in the config

// returns value from the config map
// @param key: key to the value we want to get
std::vector <std::string>	Config::get_value(const std::string& primary_key, const std::string& secondary_key)
{
	return config[primary_key][secondary_key];
}

// returns the config map
config_map	Config::get_config() const
{
	return config;
}

// output operator overload for debugging
std::ostream& operator<<(std::ostream& os, const Config& config) 
{
	config_map map = config.get_config();
	for (config_map::iterator it = map.begin(); it != map.end(); ++it)
	{
		std::cout << "primary key: " << it->first << std::endl;
		for (std::map<std::string, std::vector<std::string> >::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			std::cout << "\tsecondary key: " << it2->first << std::endl;
			for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
			{
				std::cout << "\t\t" << *it3 << std::endl;
			}
		}
	}
	return os;
}