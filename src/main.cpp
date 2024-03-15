
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
		ConfigDispatcher config(ConfigParser().get_config());
		// config.print_error_pages();
		// config.print_servers();
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
