#ifndef HEADER_HPP
#define HEADER_HPP
#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include "Parser.hpp"
#include <stack>
#include <algorithm>
#include "Config.hpp"

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"

#define EXPECTED_EXT ".conf"

#define CONF_UNEXPECTED_NL "unexpected newline - terminate value lines with ';'"
#define CONF_EMPTY "config file is empty"
#define CONF_CLOSING_BRACE "missing closing brace - terminate scope with '}'"
#define CONF_INVALID_EXT "invalid file extension (expected: .conf)"

#endif