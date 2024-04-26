#ifndef LOG_HPP 
#define LOG_HPP


#include <fstream> 
#include <string>
#include <sys/time.h>

#define LRESET          "\033[0m"
#define LLIGHT_BLUE     "\033[34m"
#define LWHITE          "\033[37m"
#define LBLINK           "\033[5m"
#define LYELLOW         "\033[33m"
#define LCYAN           "\033[36m"
#define LLIGHT_RED      "\033[91m"
#define LRED            "\033[31m"
#define LDARK_GREY      "\033[90m"
#define LLIGHTMAGENTA   "\033[95m"

enum LogDest
{
	STD_OUT = 1,
	STD_ERR = 2,
	ERROR_FILE = 4,
	ACCEPT_FILE = 8
};

class Log
{
	public:
		static bool				log(const std::string& msg, int output = STD_OUT);
		static void				init_outfiles(const std::string& error_file, const std::string& accept_file);
		static std::string		get_error_file();
		static void				set_error_file(const std::string& ferror);
		static std::string		get_accept_file();
		static void				set_accept_file(const std::string& faccept);
		static void				create_logs_directory();
		static std::string 		logmessage(const std::string& msg);
		static int				output_flag;

	private:
								Log();
								~Log();
								Log(const Log& rhs);
		Log&					operator=(const Log& rhs);
		static std::string		get_time_stamp();
		static std::ofstream	logfile;
		static std::string		error_file;
		static std::string		accept_file;
};

#endif

