#ifndef CGI_HPP
#define CGI_HPP

#include <sched.h>
#include <string>
#include <map>
#include "LocationInfo.hpp"

class CGI
{
    public:

        std::string                             execute(const std::string& script);

        CGI(const std::map<std::string, std::string>& env_map, std::vector <LocationInfo*> location);
        ~CGI();

    private:

        std::string                             _cmd;
        char**                                  _env;
        std::vector <LocationInfo*>             _locations;
        std::string                             _request_body;
        std::string                             _response_body;
        std::map <std::string, std::string>     _env_map;

        std::string                             get_cmd();
        void                                    set_pipes(int request_fd[2], int response_fd[2]);
        void                                    delete_char_array(char** arguments);
        void                                    close_pipes(int count, ...);
        char**                                  set_arguments(const std::string& args, LocationInfo*& location);
        void                                    execute_script(int request_fd[2], int response_fd[2], char** arguments);
        void                                    parent(pid_t pid, int request_fd[2], int response_fd[2], char** arguments);
        LocationInfo*                           get_location(const std::string& script);
};

#endif