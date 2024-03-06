/* 
	config class implementation
	handles loading, parsing, and accessing config settings from a file
*/

#include "../inc/Config.hpp"
#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

// default constructor: loads config from path - "default_config.conf" if no param
Config::Config(const std::string& path)
{
	if (path.substr(path.size() - 5) != ".conf") // check file extension
		throw std::runtime_error("accepted config file format: .conf");

	load_config(path);
}

void Config::load_config(const std::string& path)
{
    std::ifstream 				config_file(path.c_str());
    std::stringstream 			buffer;
    std::vector<std::string> 	keys(5);

    buffer << config_file.rdbuf();

    std::vector<std::string> split = Parser::split_keep_delimiters(buffer.str(), "{};");

	if (split[0].substr(0, 7) != "webserv")
        throw std::runtime_error("please use the correct config header (webserv)");

	if (split[1] != "{")
		throw std::runtime_error("please use '{ }' for indentation");

	nesting_level.push_back("");
    for (size_t i = 2; i < split.size(); ++i) 
	{
		if (split[i] == "{") // this part seems fine afaict
		{
            nesting_level.push_back("");

			keys[nesting_level.size() - 1] = split[i - 1];
			std::cout <<"current index: " << nesting_level.size() -1 <<" - "<< split[i - 1] << std::endl;
        } 
		else if (split[i] == "}")
		{
			if (!nesting_level.empty()) 
			{
                keys[nesting_level.size() - 1] = "";
                nesting_level.pop_back();
            }
			else
				throw std::runtime_error("extraneous closing brace in config file - fuck off");
        } 
		else if (split[i] != ";") 
		{
            std::vector<std::string> temp = Parser::split(split[i], " \t\n");
            if (!temp.empty()) 
			{
				for (size_t k = nesting_level.size(); k < keys.size() - 1 ; ++k)
            		keys[k] = "";
                for (size_t j = 1; j < temp.size(); ++j)
				{
					if (keys[0].empty())
						keys[0] = "__top_level";
					if (keys[1].empty())
						keys[1] = "__sec_level";
					if (keys[2].empty())
						keys[2] = "__server_specs";
                    config[keys[0]][keys[1]][keys[2]][temp[0]].push_back(temp[j]);
                }
            }
        }
    }
	if (nesting_level.size() != 0)
		throw std::runtime_error("missing closing brace in config - fuck off");
    // std::cout << *this;
    config_file.close();
}

// returns value from the config map
//
// @param 	primary_key: primary key for required value (TOP_LEVEL for top level values)
// @param 	secondary_key: secondary key for required value (eg: 'listen'for the port)
// @return	value: requested value
// std::vector <std::string>	Config::get_value(const std::string& primary_key, const std::string& secondary_key)
// {
// 	return config[primary_key][secondary_key];
// }

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
		std::cout << "" << BOLD << UNDERLINE << it->first << RESET << "\n" << std::endl;
		for (std::map <std::string, std::map <std::string, std::map <std::string, std::vector <std::string> > > >::iterator yo = it->second.begin(); yo != it->second.end(); yo++)
		{
			std::cout << "\t" << BOLD << UNDERLINE << yo->first << RESET << "\n" << std::endl;
			for (std::map <std::string, std::map <std::string, std::vector <std::string> > >::iterator i = yo->second.begin(); i != yo->second.end(); i++)
			{
				std::cout << "\t\t" << BOLD << UNDERLINE << i->first << RESET << "\n" << std::endl;
				for (std::map<std::string, std::vector<std::string> >::iterator it2 = i->second.begin(); it2 != i->second.end(); ++it2)
				{
					std::cout << "\t\t\t" << BOLD << it2->first << RESET << std::endl;
					for (std::vector<std::string>::iterator it3 = it2->second.begin(); it3 != it2->second.end(); ++it3)
					{
						std::cout << "\t\t\t\t" << *it3 << std::endl;
					}
				}
			}
		}
	}
	return os;
}

Config::~Config() {}

Config::Config(const Config& rhs) {*this = rhs;}

Config&	Config::operator=(const Config& rhs)
{
	if (this != &rhs)
		config = rhs.config;
	return (*this);
}