#include "Route.hpp"
#include <ostream>

Route::Route() : _directory_listing_enabled(false), _accept_file_upload(false), _http_redirect(false) {}

Route::~Route() {}

void Route::set_name(const std::string& name)
{
    _name = name;
}

std::string Route::get_name() const
{
    return _name;
}

void Route::set_root(const std::string& root)
{
    _root = root;
}

std::string Route::get_root() const
{
    return _root;
}

void Route::set_default_file(const std::string& default_file)
{
    _default_file = default_file;
}

std::string Route::get_default_file() const
{
    return _default_file;
}

void Route::set_upload_directory(const std::string& upload_directory)
{
    _upload_directory = upload_directory;
}

std::string Route::get_upload_directory() const
{
    return _upload_directory;
}

void Route::set_http_redirect(bool http_redirect)
{
    _http_redirect = http_redirect;
}

bool Route::get_http_redirect() const
{
    return _http_redirect;
}

void Route::set_allowed_methods(const std::vector <std::string>& allowed_methods)
{
    _allowed_methods = allowed_methods;
}

std::vector <std::string> Route::get_allowed_methods() const
{
    return _allowed_methods;
}

void    Route::set_directory_listing(bool directory_listing_enabled)
{
    _directory_listing_enabled = directory_listing_enabled;
}

bool    Route::directory_listing_enabled() const
{
    return _directory_listing_enabled;
}

void    Route::set_accept_file_upload(bool accept_file_upload)
{
    _accept_file_upload = accept_file_upload;
}

bool    Route::accept_file_upload() const
{
    return _accept_file_upload;
}

std::ostream &operator<<(std::ostream &os, const Route& route)
{
    if (route.get_name().empty() == true)
        return os;
    os << "route: " << route.get_name() << std::endl;
    os << "\troot: " << route.get_root() << std::endl;
    os << "\tdefault file: " << route.get_default_file() << std::endl;
    os << "\tupload directory: " << route.get_upload_directory() << std::endl;
    os << "\tallowed methods: ";
    std::vector <std::string> methods = route.get_allowed_methods();
    for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
        os << *it << " ";
    if (route.directory_listing_enabled() == true)
    {   
        os << "\n\tdirectory listing enabled: true" << std::endl;
    }
    else
    {
        os << "\n\tdirectory listing enabled: false" << std::endl;
    }
    if (route.accept_file_upload() == true)
    {    
        os << "\taccept file upload: true" << std::endl;
    }
    else
    {
        os << "\taccept file upload: false"  << std::endl;
    }
    if (route.get_http_redirect() == true)
    {
        os << "\thttp redirect: true" << std::endl;
    }
    else
    {
        os << "\thttp redirect: false" << std::endl;
    }
    return os;
}
