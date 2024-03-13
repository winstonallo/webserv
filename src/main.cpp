
#include <iostream>
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
		ConfigDispatcher ConfigDispatcher(ConfigParser().get_config());
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
