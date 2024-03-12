#include "../inc/Headers.hpp"

int main(int argc, char **argv)
{
	try
	{
		std::string config_path = "";
		if (argc == 2)
			config_path = argv[1];
		Config config(config_path);
	}
	catch (const std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}