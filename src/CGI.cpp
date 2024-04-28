#include <cstring>
#include <cstdlib>
#include <sched.h>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <sys/wait.h>
#include <cstdarg>
#include <vector>
#include "CGI.hpp"
#include "LocationInfo.hpp"
#include <sys/select.h>
#include "Utils.hpp"
#include "Request.hpp"
#include "Log.hpp"

CGI::CGI(char** env)
{
    _response_body = "";
    _env = env;
	_env = new char *[9];
    memset(_env, 0, 9 * sizeof(char*));
	_errcode = 0;
}

void    CGI::initialize_environment_map(Request& request)
{
    _env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env_map["CONTENT_LENGTH"] = request.get_header("CONTENT-LENGTH");
    _env_map["CONTENT_TYPE"] = request.get_header("CONTENT-TYPE");
    _env_map["QUERY_STRING"] = request.get_query();
    _env_map["REQUEST_METHOD"] = request.get_method();
    _env_map["SCRIPT_NAME"] = Utils::get_cgi_script_name(request.get_uri());
    _env_map["SERVER_NAME"] = request.get_host();
	_env_map["HTTP_COOKIE"] = request.get_header("COOKIE");

    int i = 0;

    for (std::map <std::string, std::string>::iterator it = _env_map.begin(); it != _env_map.end(); it++)
    {
        std::string line = it->first + "=" + it->second;
        _env[i] = new char[line.size() + 1];
        std::strncpy(_env[i], line.c_str(), line.size() + 1);
        i++;
    }
}

char** CGI::set_arguments(const std::string& command, LocationInfo*& location)
{
    char** arguments = new char*[3];

    arguments[0] = new char[location->get_cgi_handler().size() + 1];
    strcpy(arguments[0], location->get_cgi_handler().c_str());

    arguments[1] = new char[command.size() + 1];
    strcpy(arguments[1], command.c_str());

    arguments[2] = NULL;

    return arguments;
}

void    CGI::set_pipes(int request_fd[2], int response_fd[2])
{
    if (pipe(request_fd) == -1)
    {
		_errcode = 500;
		Log::log("Error. CGI Request pipe failed.", STD_ERR | ERROR_FILE);
        throw std::runtime_error("pipe() failure");
    }
    if (pipe(response_fd) == -1)
    {
		Log::log("Error. CGI Response pipe failed.", STD_ERR | ERROR_FILE);
		_errcode = 500;
        close_pipes(2, request_fd[0], request_fd[1]);
        throw std::runtime_error("pipe() failure");
    }
}

void    CGI::execute_script(int request_fd[2], int response_fd[2], char** arguments)
{
    close_pipes(2, response_fd[0], request_fd[1]);

    if (dup2(response_fd[1], STDOUT_FILENO) == -1 || dup2(request_fd[0], STDIN_FILENO) == -1)
    {
		Log::log("Error. CGI dup2 failed.", STD_ERR | ERROR_FILE);
        _exit(errno);
    }
    close_pipes(2, response_fd[1], request_fd[0]);
    _exit_status = execve(arguments[0], arguments, _env);
    delete_char_array(arguments);
    exit(_exit_status);

}

LocationInfo*    CGI::get_location(const std::string& script, std::vector <LocationInfo *> locations)
{
    size_t extension_pos = script.find_last_of(".");

    if (extension_pos == std::string::npos)
    {
        throw std::runtime_error("file extension missing in '" + script + "': could not execute");
    }

    std::string extension = script.substr(extension_pos);

    for (std::vector <LocationInfo *>::iterator it = locations.begin(); it != locations.end(); it++)
    {
        if ((*it)->get_cgi() == true && (*it)->get_cgi_extensions().empty() == false && (*it)->get_cgi_extensions()[0] == extension)
        {
            return *it;
        }
    }
    throw std::runtime_error("no valid cgi found for " + script);
}

pid_t	CGI::execute(std::vector <LocationInfo *> locations, const std::string& sfp)
{
    LocationInfo* location = get_location(_env_map["SCRIPT_NAME"], locations);

    set_pipes(request_fd, response_fd);
    pid_t pid = fork();
    if (pid == -1)
    {
        throw std::runtime_error("fork() failure");
    }
    else if (pid == 0)
    {
        char**       arguments = set_arguments(sfp, location);
        execute_script(request_fd, response_fd, arguments);
		return 0;
    }
    else
    {
		return pid;
    }
}

// variadic function to close pipes to avoid
// inflating the code with closes
void    CGI::close_pipes(int count, ...)
{
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++)
    {
        int pipe = va_arg(args, int);
        close(pipe);
    }
    va_end(args);
}



void    CGI::delete_char_array(char** arr)
{
	if (!arr)
		return ;
    for (int i = 0; arr[i]; i++)
    {
		delete[] arr[i];
    }
    delete[] arr;
}




void    CGI::clear()
{
    _response_body.clear();
    _locations.clear();
    _env_map.clear();
	_errcode = 0;
}

CGI::~CGI()
{
    delete_char_array(_env);
}

std::string& CGI::get_path()
{
	return _path;
}

void	CGI::set_path(const std::string& pa)
{
	_path = pa;
}
