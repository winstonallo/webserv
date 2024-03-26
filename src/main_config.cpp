
#include <iostream>
#include "Config.hpp"

int main()
{
	try
	{
		Config conf;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}
