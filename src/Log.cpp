#include "../inc/Log.hpp"

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
	strftime(timestr, sizeof(timestr), "%Y-%m-%m %H:%M:%S", local_time);
	return timestr;
}

std::string Log::logmessage(const std::string& msg)
{
	std::stringstream ss;
	ss << "[" << get_time_stamp() << "] - " << msg;	
	return ss.str();
}

std::ofstream	Log::logfile;	
std::string		Log::error_file = "error.log";
std::string		Log::accept_file = "accept.log";

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

