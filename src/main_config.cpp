
#include <iostream>
#include "Director.hpp"
#include "Config.hpp"

int main()
{
	try
	{
		Director director;
		Config conf;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return 0;
}
