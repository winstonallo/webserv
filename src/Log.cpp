#include "Log.hpp"
#include <map>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <pthread.h>

int Log::output_flag = 0;

std::string	Log::get_error_file()
{
	return error_file;
}

std::string Log::get_accept_file()
{
	return accept_file;
}

void	Log::set_error_file(const std::string& ferr)
{
	error_file = ferr;
}

void	Log::set_accept_file(const std::string& ferr)
{
	accept_file = ferr;
}

std::string Log::get_time_stamp()
{
	time_t cur_time;
	time(&cur_time);
	struct tm* local_time = localtime(&cur_time);
	char timestr[100];
	strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", local_time);
	return timestr;
}

std::string Log::logmessage(const std::string& msg)
{
	std::stringstream ss;
	ss << "[" << get_time_stamp() << "] - " << msg;
	return ss.str();
}

std::ofstream	Log::logfile;
std::string		Log::error_file = "./logs/error.log";
std::string		Log::accept_file = "./logs/accept.log";

void	Log::create_logs_directory()
{
	if (mkdir("./logs", 0755) == -1)
	{
		if (errno != EEXIST)
		{
			std::cerr << "Error creating log directory." << std::endl;
		}
	}
}

void* async_log(void* args)
{
    std::string* message = static_cast<std::string*>(args);
    std::map<int, std::string> filemapping;
    filemapping.insert(std::make_pair(STD_OUT, "stdout"));
    filemapping.insert(std::make_pair(STD_ERR, "stderr"));
    filemapping.insert(std::make_pair(ERROR_FILE, Log::get_error_file()));
    filemapping.insert(std::make_pair(ACCEPT_FILE, Log::get_accept_file()));

    for (std::map<int, std::string>::iterator it = filemapping.begin(); it != filemapping.end(); it++)
	{
        if (Log::output_flag & it->first)
		{
            std::string log_msg = Log::logmessage(*message);
			
			if (it->first == STD_OUT)
			{
                std::cout << log_msg;
            } 
			else if (it->first == STD_ERR)
			{
                std::cerr << log_msg;
            }
			else 
			{
                std::ofstream logfile;
                logfile.open(it->second.c_str(), std::ios::app);
				if (logfile.is_open()) 
				{
                    logfile << log_msg;
                    logfile.close();
                }
				else 
				{
                    std::cerr << "Error opening log file: " << it->second << std::endl;
                }
            }
        }
    }
    delete message;
    return NULL;
}

bool Log::log(const std::string& msg, int output) 
{
    pthread_t logging_thread;
    std::string* message = new std::string(msg);

    Log::output_flag = output;

    int result = pthread_create(&logging_thread, NULL, async_log, (void*)message);
    if (result != 0)
	{
        std::cerr << "Error creating logging thread." << std::endl;
        delete message;
        return false;
    }

    pthread_join(logging_thread, NULL);
    return true;
}
