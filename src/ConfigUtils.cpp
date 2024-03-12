#include "../inc/ConfigUtils.hpp"

bool	ConfigUtils::file_exists(const std::string& path)
{
	std::ifstream	file(path.c_str());

	return file.good();
}