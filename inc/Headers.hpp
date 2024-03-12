#ifndef HEADER_HPP
#define HEADER_HPP
#pragma once

#include <iostream>
#include <cerrno>
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
#include "ConfigUtils.hpp"
#include <clocale>
#include <cstring>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"

#define EXPECTED_EXT ".conf"

#define UNINITIALIZED_SCOPE "uninitialized scope: expected identifier"             
#define UNEXPECTED_NL "unexpected newline - terminate value lines with ';'"
#define UNTERM_VALUE_SCOPE "unterminated value scope - terminate value lines with ';'"
#define EMPTY "config file is empty"
#define MISSING_CLOSING_BRACE "missing closing brace - terminate scope with '}'"
#define EXTRA_CLOSING_BRACE "extraneous closing brace"
#define MISSING_OPENING_BRACE "missing opening brace - use '{ }' for nesting"
#define INVALID_EXT "invalid file extension (expected: '.conf')"
#define INV_HEADER "invalid config file header (expected: 'webserv')"
#define UNCLOSED_QUOTE "unclosed quote sequence"

#define NOT_FOUND "could not open file"


#define FALLBACK "falling back to default config: '\033[1mconfig/webserv.conf\033[0m'"

#define RULES "refer to '\033[1mconfig/rules/config-formatting-rules.md\033[0m' for more details"


#endif