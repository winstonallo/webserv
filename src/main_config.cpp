
#include <iostream>
#include "Director.hpp"
#include "Config.hpp"
#include "Route.hpp"

int main()
{
	try
	{
		Director director;
		Config conf;
		std::vector <Route*> routes = conf.get_routes();
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
