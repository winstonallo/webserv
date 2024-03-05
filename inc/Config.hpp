#ifndef CONFIG_HPP
#define CONFIG_HPP
#pragma once

#include "Headers.hpp"

#define TOP_LEVEL "__top_level"

typedef std::map <std::string, std::map <std::string, std::vector <std::string> > > config_map;

class Config 
{
	public:
	
		Config(const std::string& path="default_config.conf");
		~Config();

		void									load_config(const std::string& config_path);
		void									map_line(const std::string& line);
		void									set_server_names();
		void									set_allowed_methods();

		std::vector <std::string>				get_value(const std::string& primary_key, const std::string& secondary_key);
		config_map							 	get_config() const;
		std::vector <std::string>				get_server_names() const;
		std::vector <std::string>				get_allowed_methods() const;
		void									store_values();

	private:
	
		config_map								config;
		
		Config(const Config& rhs);
		Config&	operator=(const Config& rhs);
};

std::ostream& operator<<(std::ostream& os, const Config& config);

#endif