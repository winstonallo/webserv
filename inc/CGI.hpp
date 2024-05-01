#ifndef CGI_HPP
#define CGI_HPP

#include <sched.h>
#include <string>
#include <map>
#include <vector>
#include "LocationInfo.hpp"
#include "Request.hpp"

class CGI
{
    public:

        pid_t		                            execute(std::vector <LocationInfo *> locations, const std::string& sfp);
        void                                    initialize_environment_map(Request& request);
        void                                    clear();

        CGI(char** env=NULL);
        ~CGI();
		int										request_fd[2];
		int										response_fd[2];
		int 									get_error_code() const;
		std::string&							get_path();
		void									set_path(const std::string& pa);

    private:

        char**                                  _env;
        std::vector <LocationInfo*>             _locations;
        std::string                             _response_body;
        std::map <std::string, std::string>     _env_map;
		int										_errcode;
		int										_exit_status;
		std::string								_path;

        void                                    set_pipes(int request_fd[2], int response_fd[2]);
        void                                    delete_char_array(char** arguments);
        void                                    close_pipes(int count, ...);
        char**                                  set_arguments(const std::string& args, LocationInfo*& location);
        void                                    execute_script(int request_fd[2], int response_fd[2], char** arguments);
        void                                    parent(pid_t pid, int request_fd[2], int response_fd[2], char** arguments);
        LocationInfo*                           get_location(const std::string& script, const std::vector <LocationInfo *> locations);
};

#endif
