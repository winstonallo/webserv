#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <map>
#include "LocationInfo.hpp"

class CGI
{
    public:

        std::string                             get_cmd();
        std::string                             execute(const std::string& script);
        void                                    set_pipes(int request_fd[2], int response_fd[2]);
        void                                    delete_char_array(char** arguments);
        void                                    execute_script(int request_fd[2], int response_fd[2], char** arguments);

        CGI(const std::map<std::string, std::string>& env, LocationInfo* location);
        ~CGI();

    private:

        std::string                             _cmd;
        char**                                  _env;
        LocationInfo*                           _location;
        std::string                             _extension;
        std::string                             _request_body;
        std::string                             _response_body;
        std::map <std::string, std::string>     _env_map;

        void                                    close_pipes(int count, ...);
        char**                                  set_arguments(const std::string& args);
};

#endif