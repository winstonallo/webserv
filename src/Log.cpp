#include "Log.hpp"
#include <iostream>
#include <sstream>
#include <map>
#include <sys/stat.h>

Log::Log()
{}

Log::~Log()
{}

Log::Log(const Log& rhs)
{
	*this = rhs;
}

Log& Log::operator=(const Log& rhs)
{
	if (this != &rhs)
	{

	}
	return (*this);
}

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
//purpose:	sets the accept log file to a new file path
//
//argument: ferr -> string to the new accept file.
void	Log::set_accept_file(const std::string& ferr)
{
	accept_file = ferr;
}

//purpose:	to get the current time / date in a format
//			Year-Month-Day Hour:Minutes:Seconds
//
//return string -> Date string
std::string Log::get_time_stamp()
{
	time_t cur_time;
	time(&cur_time);
	struct tm* local_time = localtime(&cur_time);
	char timestr[100];
	strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", local_time);
	return timestr;
}

//purpose:	the formatting of the log messages
//
//argument: msg -> The message for the log
//
//return: string -> the final log string
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

//purpose: The logging of various error, accept, information
//			messages to different outputs. You can output to several outputs
//			by (OR)ing the bit flags. e.g. STD_ERR | ERROR_FILE
//
//argument: msg -> the message to be logged
//
//argument: output -> a bit flag (STD_OUT, STD_ERR, ERROR_FILE, ACCEPT_FILE)
//			for the output of the log
//			STD_OUT for cout on the console
//			STD_ERR for cerr on the console
//			ERROR_FILE to log to the given error_file (default: error.log)
//			ACCEPT_FILE to log to the given accept_file (defaul: accept.log)
//
//return: bool -> true if succeded or false if failed.
bool	Log::log(const std::string& msg, int output)
{
	std::map<int, std::string> filemapping;
	filemapping.insert(std::make_pair(1, "stdout"));
	filemapping.insert(std::make_pair(2, "stderr"));
	filemapping.insert(std::make_pair(ERROR_FILE, error_file));
	filemapping.insert(std::make_pair(ACCEPT_FILE, accept_file));
	std::map<int, std::string>::iterator e = filemapping.end();
	std::map<int, std::string>::iterator it;
	for (it = filemapping.begin(); it != e; it++)
	{
		if (output & it->first)
		{
			if (it->first == 1)
			{
				std::cout << logmessage(msg);
				continue;
			}
			if (it->first == 2)
			{
				std::cerr << logmessage(msg);
				continue;
			}
			logfile.open((it->second).c_str(), std::ios::app);
			if (logfile.is_open())
			{
				logfile << logmessage(msg);
				logfile.close();
			}
			else
			{
				std::cerr << "Error opening error log file." << std::endl;
				return false;
			}
		}
	}
	return true;
}
