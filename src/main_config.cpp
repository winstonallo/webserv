
#include <iostream>
#include "Config.hpp"

int main()
{
	try
	{
		Config conf;
		std::cout << conf;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
