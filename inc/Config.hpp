#ifndef CONFIG_HPP
#define CONFIG_HPP
#include <cstddef>
#include <vector>
#pragma once

#include "Headers.hpp"

typedef std::map <std::string, std::map <std::string, std::map <std::string, std::map <std::string, std::vector <std::string> > > > > config_map;
class Config 
{
	public:
	
		Config(const std::string& path="webserv.conf");
		~Config();

		void									load_config(const std::string& config_path);
		void									load_file_to_map(std::ifstream& config_file);
		void									get_primary_key(const std::string& line, const std::string& key);
		void									set_server_names();
		void									set_allowed_methods();

		std::vector <std::string>				get_value(const std::string& primary_key, const std::string& secondary_key);
		config_map							 	get_config() const;
		std::vector <std::string>				get_server_names() const;
		std::vector <std::string>				get_allowed_methods() const;
		void									store_values();

	private:
	
		config_map								config;				// see typedef above map
		std::stack <std::string> 				nesting_level;		// keeps track of nesting level

		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif