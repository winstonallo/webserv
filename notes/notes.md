# Error tests:
## Config File:
		- Can we write a directive (like listen 8080) several times? (we shouldn't)
		- Check if token after Location is empty (and the next token is '{)
		- Check if token after path is {
		- Check if root in Location is a real path
		- Check if allow_methods is written twice
		- Put a ; inbetween multiple values
		- is autoindex allowed in /cgi-bin Location?
		- is autoindex allowed more than once?
		- is alias allowed in /cgi-bin
		- is alias allowed twice?
		- check if after directive there can be empty values
		- if cgi check if the cgi_handler is python or bash
		- are the location index locations set from parent scope if there is no index in Location?
		- is the location max_body_size set to the out scope ones if it is emtpy?
		- can you have a /cgi-bin path without setting cgi_handler, cgi_extension, index_location?
		- is Location path a valid one?
		- are the cgi-handlers real files?
		- can you write *.py and *.sh for extension?
		- can Location path start without / ?
		- is Location root set from outer scope?
		- is the path location + index a file that is readable
		- does return exist?
		- does alias exists and readable?
		- can host be written after a Location?
		- can we have multiple hosts in one server?
		- same with root error_page
		- is server_name, max_body_size duplicated?
		- can autoindex be different than on or off?
		- if the following arent set are they getting a default value? root(/), host(localhost), index(index.html)
		- are Location checked for duplicates?
		- is there a port?
		- are the error pages valid?
		- can there be two servers with same host, port and server_name?
		- on two servers. is port host and server_name the same?

## Request:
		- does path of Request line start with / ?
		- are the chars in the URI allowed?
		- do we check for the maximum uri length?
		- do we check for HTTP with / ?
		- do we check for right version (are they numbers)?
		- are we checking that the line ends with \r\n?
		- do the headers have valid characters?
		- do we check if the transfer-encoding is chunked?
		- do we set the server name from the host?

## CGI:
		- is path ending with .py or .sh
		- is it a file?
		- do we have execute permission?
		- is the method of the request allowed?
		- is the CGI class reset before a new response?
		- in the config file, can we put cgi extensions and such in a Location that is not in the cgi-bin path?

## Response:
		- Do we redirect when the request path is a directory without a final / ?
		- do we replace with alias?
		- if we have a directory path, do we append index of location or index of server?
		- do we check if the file to be returned exist?
