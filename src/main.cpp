#include "Log.hpp"
#include "Server.hpp"
#include "Director.hpp"
#include <exception>
#include <signal.h>

void	pipe_signal_handler(int signal)
{
	if (signal)
	{
		return ;
	}
}

bool interrupted = false;

void	ctrlhandler(int)
{
	interrupted = true;
}

int main(int argc, char **argv)
{
	try 
	{
		Log::create_logs_directory();

		if (argc != 1 && argc != 2)
		{
			std::cerr << "Error. Invalid number of arguments." << std::endl;
			std::cerr << "Usage: " << argv[0] << " [config file <.conf>]" << std::endl;
			return 1;
		}
		else
		{
			signal(SIGPIPE, pipe_signal_handler);
			signal(SIGINT, ctrlhandler);
			std::string	config_file;
			if (argc == 2)
			{
				config_file = argv[1];
			}
			else
			{
				config_file = "assets/config/webserv.conf";
			}

			Director director(config_file);

			if(director.init_servers() < 0)
			{
				std::cerr << "Error initializing servers." << std::endl;
				return 1;
			}
			if (director.run_servers() < 0)
			{
				return 1;
			}
		}
	}
	catch (const std::exception& e)
	{
		return 1;
	}
	return 0;
}
