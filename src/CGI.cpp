#include "CGI.hpp"
#include <ostream>

CGI::CGI() {}

CGI::~CGI() {}

void CGI::set_name(const std::string& name)
{
    _name = name;
}

std::string CGI::get_name() const
{
    return _name;
}

void CGI::set_extension(const std::vector <std::string>& extension)
{
    _extension = extension[0];
}

std::string CGI::get_extension() const
{
    return _extension;
}

void CGI::set_handler(const std::vector <std::string>& handler)
{
    _handler = handler[0];
}

std::string CGI::get_handler() const
{
    return _handler;
}

void CGI::set_allowed_methods(const std::vector <std::string>& allowed_methods)
{
    if (allowed_methods.empty() == false)
    {
        _allowed_methods = allowed_methods;
    }
}

std::vector <std::string> CGI::get_allowed_methods() const
{
    return _allowed_methods;
}

std::ostream &operator<<(std::ostream& out, const CGI& rhs)
{
    out << "name: " << rhs.get_name() << std::endl;
    out << "\textension: " << rhs.get_extension() << std::endl;
    out << "\thandler: " << rhs.get_handler() << std::endl;
    out << "\tallowed methods: ";
    for (size_t i = 0; i < rhs.get_allowed_methods().size(); i++)
    {
        out << rhs.get_allowed_methods()[i] << " ";
    }
    out << std::endl;
    return out;
}