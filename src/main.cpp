#include "Log.hpp"
#include "Server.hpp"
#include "Director.hpp"
#include <exception>
#include <signal.h>
#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>


void	pipe_signal_handler(int signal)
{
	if (signal)
	{
		return ;
	}
}

bool interrupted = false;

void	sigint_handler(int)
{
	interrupted = true;
}

Director	*webserv_init(int argc, char** argv)
{
	if (argc > 2)
	{
		std::string	error_message = "Error: Invalid number of arguments\nUsage: ./webserv [optional]<*.conf>\n";
		
		throw std::runtime_error(error_message);
	}
	std::string config_file_path = (argc == 2 ? argv[1] : "assets/config/webserv.conf");
	Director* director = new Director(config_file_path);

	Log::create_logs_directory();
	
	signal(SIGPIPE, pipe_signal_handler);
	signal(SIGINT, sigint_handler);

	return director;
}

void closeAllFileDescriptors() {
    for (int fd = 3; fd < 1024; ++fd) {
        if (fcntl(fd, F_GETFD) != -1 || errno != EBADF) {
            close(fd);
        }
    }
}


int main(int argc, char **argv)
{
	Director* director = NULL;

	try
	{
		director = webserv_init(argc, argv);

		director->init_servers();
		director->run_servers();
	}
	catch (const std::exception& e)
	{
		Log::log(e.what(), ERROR_FILE | STD_ERR);

		if (director != NULL)
		{
			delete director;
		}
		return 1;
	}

	delete director;
	closeAllFileDescriptors();
	return 0;
}
