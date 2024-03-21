#include "Route.hpp"

Route::Route() {}

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

void Route::set_http_redirect(const std::string& http_redirect)
{
    _http_redirect = http_redirect;
}

std::string Route::get_http_redirect() const
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