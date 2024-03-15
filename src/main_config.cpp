
#include <iostream>
#include <vector>
#include "Director.hpp"
#include "Config.hpp"
#include "ConfigParser.hpp"
#include "ConfigDispatcher.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	try
	{
		Config conf;
		Director director;
		ConfigDispatcher config(ConfigParser().get_config());
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
