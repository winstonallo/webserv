#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <vector>

class Route
{
    public:
    
        Route();
        ~Route();

        void                        set_name(const std::string& name);
        std::string                 get_name() const;
        void                        set_root(const std::string& root);
        std::string                 get_root() const;
        void                        set_default_file(const std::string& default_file);
        std::string                 get_default_file() const;
        void                        set_upload_directory(const std::string& upload_directory);
        std::string                 get_upload_directory() const;
        void                        set_http_redirect(const std::string& http_redirect);
        std::string                 get_http_redirect() const;
        void                        set_allowed_methods(const std::vector <std::string>& allowed_methods);
        std::vector <std::string>   get_allowed_methods() const;
    
    private:

        std::string                 _name;
        std::string                 _root;
        std::string                 _default_file;
        std::string                 _upload_directory;
        std::string                 _http_redirect;
        std::vector <std::string>   _allowed_methods;

        Route(const Route& rhs);
        Route &operator=(const Route& rhs);
};

#endif