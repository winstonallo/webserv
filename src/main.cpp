#include "../inc/Config.hpp"
#include <exception>

int main()
{
	try
	{
		Config config;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
	}
}