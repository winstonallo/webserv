
#include <iostream>
#include "ServerInfo.hpp"
#include "Director.hpp"
#include "Config.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	try
	{
		Config config;
		std::map <int, std::map <std::string, std::vector <std::string> > > servers = config.get_servers();
		for (std::map <int, std::map <std::string, std::vector <std::string> > >::iterator it = servers.begin(); it != servers.end(); it++)
		{
			std::cout << it->first << ": " << std::endl;
			for (std::map <std::string, std::vector <std::string> >::iterator itt = it->second.begin(); itt != it->second.end(); itt++)
			{
				std::cout << "\t" << itt->first << ": " << std::endl;
				for (size_t i = 0; i < itt->second.size(); i++)
				{
					std::cout << "\t\t" << itt->second[i] << std::endl;
				}
			}
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
