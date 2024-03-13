#include "Log.hpp"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	Log::log("Hello my friend!");
	Log::log("Error message", STD_ERR);
	Log::log("This is an general log", ERROR_FILE | ACCEPT_FILE | STD_ERR | STD_OUT);
	Log::log("This is an accept log",STD_OUT);
	Log::set_error_file("new_error_file.log");
	Log::log("Message on the new_error_file.log", ERROR_FILE);
}