#include "../inc/Config.hpp"

int main()
{
	try
	{
		Config config;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}