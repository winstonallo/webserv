#include "CGI.hpp"
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <sys/wait.h>
#include <cstdarg>
#include "Log.hpp"
#include "Utils.hpp"

CGI::CGI(const std::map<std::string, std::string>& env_map, LocationInfo* location)
{
    _env_map = env_map;
    _response_body = "";
    _location = location;
    _env = new char*[_env_map.size() + 1];

    _env[_env_map.size()] = NULL;
    int i = 0;

    for (std::map <std::string, std::string>::iterator it = _env_map.begin(); it != _env_map.end(); it++)
    {
        std::string line = it->first + "=" + it->second;
        _env[i] = new char[line.size() + 1];
        std::strncpy(_env[i], line.c_str(), line.size() + 1);
        i++;
    }
}

std::string    CGI::get_cmd()
{
    return ""; // TODO: implement this
}

char**    CGI::set_arguments(const std::string& command)
{
    char** arguments = new char*[3];

    std::string interpreter = _location->get_cgi_path();
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

std::string CGI::execute(const std::string& script)
{
    char**      arguments = set_arguments(script);
    int         request_fd[2], response_fd[2];

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
        delete_char_array(arguments);
        close_pipes(1, request_fd[0]);
        write(request_fd[1], _request_body.c_str(), _request_body.size());
        close_pipes(2, request_fd[1], response_fd[1]);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) == true)
        {
            int exit_status = WEXITSTATUS(status);
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