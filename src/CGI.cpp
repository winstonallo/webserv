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
#include "Log.hpp"
#include "Utils.hpp"
#include "Request.hpp"

CGI::CGI(char** env)
{
    _response_body = "";
    _env = env;
}

void    CGI::initialize_environment_map(Request& request)
{
    _env_map["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env_map["CONTENT_LENGTH"] = request.get_header("Content-length");
    _env_map["CONTENT_TYPE"] = request.get_header("Content-Type");
    _env_map["QUERY_STRING"] = request.get_query();
    _env_map["REMOTE_HOST"] = request.get_host();
    _env_map["REQUEST_METHOD"] = request.get_method();
    _env_map["SCRIPT_NAME"] = Utils::get_cgi_script_name(request.get_uri());

    int i = 0;

    for (std::map <std::string, std::string>::iterator it = _env_map.begin(); _env[i] && it != _env_map.end(); it++)
    {
        std::string line = it->first + "=" + it->second;
        _env[i] = new char[line.size() + 1];
        std::strncpy(_env[i], line.c_str(), line.size() + 1);
        i++;
    }
}

char**    CGI::set_arguments(const std::string& command, LocationInfo*& location)
{
    char** arguments = new char*[3];

    std::string interpreter = location->get_cgi_handler();
    arguments[0] = new char[interpreter.size() + 1];
    std::strncpy(arguments[0], interpreter.c_str(), interpreter.size() + 1);
    arguments[1] = new char[command.size() + 1];
    std::strncpy(arguments[1], command.c_str(), command.size() + 1);
    arguments[2] = NULL;


    return arguments;
}

void    CGI::set_pipes(int request_fd[2], int response_fd[2])
{
    if (pipe(request_fd) == -1)
    {
        throw std::runtime_error("pipe() failure");
    }
    if (pipe(response_fd) == -1)
    {
        close_pipes(2, request_fd[0], request_fd[0]);
        throw std::runtime_error("pipe() failure");
    }
}

void    CGI::execute_script(int request_fd[2], int response_fd[2], char** arguments)
{
    close_pipes(2, response_fd[0], request_fd[1]);

    if (dup2(response_fd[1], STDOUT_FILENO) == -1 || dup2(request_fd[0], STDIN_FILENO) == -1)
    {
        _exit(errno);
    }
    close_pipes(2, response_fd[1], request_fd[0]);

    execve(arguments[0], arguments, _env);
    delete_char_array(arguments);
    _exit(errno);
}

void    CGI::parent(pid_t pid, int request_fd[2], int response_fd[2], char** arguments)
{
        delete_char_array(arguments);
        close_pipes(1, request_fd[0]);
        write(request_fd[1], _request_body.c_str(), _request_body.size());
        close_pipes(2, request_fd[1], response_fd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) == true)
        {
            int exit_status = WEXITSTATUS(status);
            std::cout << "exit status: " << exit_status << std::endl;
            if (exit_status != 0)
            {
                Log::log("error: child process exited with status " + Utils::itoa(exit_status) + ": " + strerror(exit_status));
            }
        }
        char buffer[1024];
        int ret = 0;
        do
        {
            std::memset(buffer, 0, 1024);
            ret = read(response_fd[0], buffer, 1024);
            _response_body.append(buffer, ret);
        }
        while(ret > 0);

        close_pipes(1, response_fd[0]);
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

std::string CGI::execute(std::vector <LocationInfo *> locations)
{
    LocationInfo* location = get_location(_env_map["SCRIPT_NAME"], locations);
    int          request_fd[2], response_fd[2];
    char**       arguments = set_arguments(_env_map["SCRIPT_NAME"], location);

    set_pipes(request_fd, response_fd);
    pid_t pid = fork();
    if (pid == -1)
    {
        throw std::runtime_error("fork() failure");
    }
    else if (pid == 0)
    {
        execute_script(request_fd, response_fd, arguments);
    }
    else
    {
        parent(pid, request_fd, response_fd, arguments);
    }
    return _response_body;
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
    for (int i = 0; arr[i]; i++)
    {
        delete[] arr[i];
    }
    delete[] arr;
}

CGI::~CGI()
{
    delete_char_array(_env);
}
