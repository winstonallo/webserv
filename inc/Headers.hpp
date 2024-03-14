#ifndef HEADER_HPP
#define HEADER_HPP

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
#include <stack>
#include <algorithm>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>
#include <clocale>
#include <cstring>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Node.hpp"
#include "ServerInfo.hpp"
#include "ClientInfo.hpp"
#include "Config.hpp"
#include "Log.hpp"
#include "Utils.hpp"
#include "LocationInfo.hpp"
#include "Director.hpp"

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

#define BOLD "\033[1m"
#define RESET "\033[0m"
#define UNDERLINE "\033[4m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"

#endif