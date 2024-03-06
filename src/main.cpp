#include "../inc/Headers.hpp"

int main(int argc, char **argv)
{
	try
	{
		if (argc == 2)
			Config config(argv[1]);
		else
			Config config;
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}