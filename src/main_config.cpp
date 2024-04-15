
#include <iostream>
#include "Config.hpp"

int main(int argc, char **argv)
{
	try
	{
		(void)argc;
		Config conf((argv[1]));
		std::cout << conf;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
