
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
		std::map<int, std::map<std::string, std::vector<std::string> > > server_map = config.get_servers();
		conf.set_servers(server_map);
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
