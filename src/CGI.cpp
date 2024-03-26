#include "CGI.hpp"
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <stdexcept>
#include <cerrno>
#include <sys/wait.h>

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

std::string CGI::execute(const std::string& script)
{
    char**      arguments = set_arguments(script);
    int         pipe_request[2], pipe_response[2];
    (void)arguments;

    if (pipe(pipe_request) == -1)
    {
        throw std::runtime_error("pipe() failure");
    }
    if (pipe(pipe_response) == -1)
    {
        close(pipe_request[0]);
        close(pipe_request[1]);
        throw std::runtime_error("pipe() failure");
    }

    pid_t pid = fork();
    if (pid == -1)
    {
        throw std::runtime_error("fork() failure");
    }
    else if (pid == 0)
    {
        close(pipe_response[0]);
        close(pipe_request[1]);

        if (dup2(pipe_response[1], STDOUT_FILENO) == -1 || dup2(pipe_request[0], STDIN_FILENO) == -1)
        {
            _exit(errno);
        }

        close(pipe_response[1]);
        close(pipe_request[0]);

        execve(arguments[0], arguments, _env);
        for (int i = 0; arguments[i]; i++)
        {
            delete[] arguments[i];
        }
        delete[] arguments;
        _exit(errno);
    }
    else
    {
        for (int i = 0; arguments[i]; i++)
        {
            delete[] arguments[i];
        }
        delete[] arguments;
        close(pipe_request[0]);
        write(pipe_request[1], _request_body.c_str(), _request_body.size());
        close(pipe_request[1]);
        close(pipe_response[1]);

        int status;
        waitpid(pid, &status, 0);
        char buffer[1024];
        int ret = 0;
        do 
        {
            ::memset(buffer, 0, 1024);
            ret = read(pipe_response[0], buffer, 1024);
            _response_body.append(buffer, ret);
        }
        while(ret > 0);

        close(pipe_response[0]);
    }
    return _response_body;
}

CGI::~CGI()
{
    for (int i = 0; _env[i]; i++)
    {
        delete[] _env[i];
    }
    delete[] _env;
}