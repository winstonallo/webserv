
#include <iostream>
#include <vector>
#include "ServerInfo.hpp"
#include "Director.hpp"
#include "ConfigParser.hpp"
#include "ConfigDispatcher.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	try
	{
		Director director;
		ConfigDispatcher config(ConfigParser().get_config());
		// std::cout << BOLD << "-----------------------------error-pages-----------------------------" << RESET << std::endl;
		// config.print_error_pages();
		// std::cout << BOLD << "-----------------------------s-e-rve-r-s-----------------------------" << RESET << std::endl;
		// config.print_servers();
		// std::cout << BOLD << "-----------------------------r-o-u-t-e-s-----------------------------" << RESET << std::endl;
		// config.print_routes();
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
