#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <vector>

class CGI
{
    public:

        CGI();
        ~CGI();
    
        void                            set_name(const std::string& name);
        std::string                     get_name() const;
        void                            set_extension(const std::vector <std::string>& extension);
        std::string                     get_extension() const;
        void                            set_handler(const std::vector <std::string>& extension);
        std::string                     get_handler() const;
        void                            set_allowed_methods(const std::vector <std::string>& extension);
        std::vector <std::string>       get_allowed_methods() const;

    private:

        std::string                     _name;
        std::string                     _extension;
        std::string                     _handler;
        std::vector <std::string>       _allowed_methods;

        CGI(const CGI& rhs);
        CGI &operator=(const CGI& rhs);
};

#endif