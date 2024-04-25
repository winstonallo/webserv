# expected config format
invalid custom config will result in fallback to 'config/webserv.conf'
## file format
* **config file is empty**: no values in config file
* **invalid file extension**: expecting .conf, anything else is invalid
## header
#### error messages
* **invalid config file header:** the webserv config is to be prefixed by the keyword **`webserv`**
* **missing opening brace:** the header must be directly followed (excluding comments & whitespaces) by an opening brace **`{`**.
## comments
every character following **`#`** will be ignored in parsing until the next newline.
## scopes
### declare a scope
a new scope is to be opened/closed by **`{}`** and declared with a name, eg:

```conf
/admin # this will be used as the scope key
{ # opening brace
	root	/tmp; # value level
} # closing brace
```
#### error messages:
* **uninitialized scope:** failure to initialize a scope with a name will cause the configuration file to be considered invalid:

**invalid**:
```conf
...                                       
} # opening new scope with no identifier
{                                        
	root	/tmp;      
}                        
```
**valid**:
```conf
...
/admin # this is needed for storing the values
{
	root	/tmp;
}
```
this can be simplified to:

```cpp
if (line == "{" && prev_line.is_delimiter() == true)
{
	config = denied;
}	
```
* **missing closing brace**: scope(s) left unclosed

any unclosed scope can lead to values being stored incorrectly -> undefined behavior, thus:
```cpp
if (num_of_opening_braces > num_of_closing_braces)
{
	config = denied;
}
```
* **extraneous closing brace**: too many closing braces
```cpp
if (num_of_opening_braces < num_of_closing_braces)
{
	config = denied;
}
```
### value level:
a line ended (excluding comments) by **`;`** will be parsed as value level, eg:

`allowed_methods	POST GET;`

the line will be split into words (by whitespaces), the first word being considered the key & the following words the values.

thus, a line followed by **`;`** & containing less than 2 words will be considered invalid and ignored in parsing.
#### error messages:
* **unterminated value scope**: value line not closed by **';'** before leaving the parent scope
```conf
webserv
{
	port			80;
	server_name		domain.com       # < no semicolon before closing brace
}
```
* **unexpected newline**: value line not closed by **';'** before moving to the next line
```conf
webserv
{
	port			80				# < no semicolon before new line
	server_name		domain.com;      
}
```
### duplicate keys within scope
duplicate keys within the same scope will be merged, eg:
```conf
webserv
{
	port			80;
	server_name		domain.com;
	host			localhost;
	server_name		www.domain.com;
}
```
**-->** `config["webserv:server_name"] = "domain.com", "www.domain.com"`
### empty scopes
empty scopes, eg:
```conf
routes
{

}
```
will be ignored in parsing and have no effect on the configuration of the server.
## characters
### special characters
the following characters are reserved:
* '**;**'
* '**{**'
* '**}**'
* '**#**'

they can be escaped using double quotes if needed in a value
### escape characters
any escape like **`\t`**, **`\n`**, **`\\`** will be parsed as literals, eg:

`host	local\nhost;` will be read as `config["webserv:host"] = "localhost"`
## multi-word tokens
any token containing whitespaces ('\n', '\t' or ' ') is to be enclosed by double quotes (**"**).
#### error messages: 
* **unclosed quote sequence**: quote sequence left open