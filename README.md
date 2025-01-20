# webserv
This project's aim is to build a webserver adhering to the HTTP/1.1 protocol. 

## How to Run

### Requirements
* `make`
* `C++98`

### Execution
```sh
git clone git@github.com:winstonallo/webserv.git
cd webserv
make
./webserv
```
The logs printed to the console will show you which URL to look up in order to look at the static website we programmed for testing purposes.

The [default configuration](assets/config/webserv.conf) shows many of the implemented features.
#### Links
Once executed **with the default configuration**, follow these links to test functionality:
* [CGI (Python)](http://localhost:8080/cgi-bin/date.py)
* [CGI (Bash)](http://localhost:8080/cgi-bin/do_shell.sh)
* [File upload and deletion](http://localhost:8080/tests.html)
* [Session management](http://localhost:8080/cgi-bin/session.py)

## Requirements
`webserv` must:
* Support non-blocking I/O operations for up to 1024 concurrent clients using only one `select` call, which must check for reads and writes at the same time.
* Support client timeout in case oh hanging requests.
* Have accurate response status codes.
* Generate default error pages if none are provided in the configuration.
* Be able to serve a fully static website.
* Support file uploads.
* Implement at least `GET`, `POST` and `DELETE` methods.
* Keep 100% availability during stress tests with `siege`.
* Be able to listen to multiple ports.
* Support configuration:
    * Choose the port and host of each server.
    * Set up server names (or not).
    * Set up default error pages.
    * Limit client body size.
    * Set up locations with one or multiple of the following rules:
        * Define a list of allowed methods for the location.
        * Define a HTTP redirection.
        * Define a root directory.
        * Define an index file.
        * Enable/Disable directory listing.
        * Execute CGI.
        * Enable file upload, including target upload directory.
* Support cookies.
* Support session management.
* Handle multiple CGI interpreters.

## Configuration
We chose a configuration format inspired by Nginx. [Here](assets/config/webserv.conf) is a fully-fledged configuration for running a test server, serving static files and showing execution of CGI scripts.

### Rules
The whole configuration must be encapsulated in a `webserv` block as follows:
```
webserv {
    ...
}
```
Inside of the block, any amount of servers can be configured: 
```
webserv {
    server {
        ...
    }

    server {
        ...
    }
}
```
### Options
#### Server Block 
Each server block has multiple levels of configuration. The only required options are: 
```
webserv {
    server {
        host        localhost;          # Host, localhost or IP address.
        port        8080; 
    }
}
```
Every other configuration option has a default value and can therefore be left out.

| Option   | Description| Default Value |
| -------- | -------- |------- |
| `server_name` | Host name of the server | `default.com` |
| `access_log` | Access log path | `access.log` |
| `client_max_body_size` | Maximum request body size | `1M` |
| `index` | Default file to serve | ` ` |
| `root` | Root of server (all paths will be relative to this) | `/` |
| `allowed_methods` | List of HTTP methods to allow for this server | `GET` |
| `autoindex` | Whether to list the content of the server's root (only applicable if `index` is not set) | `false` |

#### Location Block
For more fine-grained configuration, a specific location inside a server can be configured to have different rules. The syntax is as follows:
```
webserv {
    server {
        host        localhost;
        port        8080;
        root        /root/;

        location /data { 
            ...
        }
    }
}
```
The location path is relative to the server's root - above examples expanded path will be `/root/data`. Any configuration options set here will override the server's configuration for this location.

| Option   | Description| Default Value |
| -------- | -------- |------- |
| `index` | Default file to serve | ` ` |
| `root` | Root of server (all paths will be relative to this) | `/` |
| `allowed_methods` | List of HTTP methods to allow for this server | `GET` |
| `autoindex` | Whether to list the content of the server's root (only applicable if `index` is not set) | `false` |
| `return` | Specify a redirect path and status code | `null` |
| `alias` | Define another directory the location should map to | `null` |

#### Location Block: CGI
A location block pointing to the `/cgi-bin` location will be treated as a CGI block, which supports setting a handler (e.g., `/usr/bin/python3`) and an extension (e.g., `.py`), additionally to all other location configuration options.

When receiving a request for a file ending with `extension`, the server will try to execute it with `handler`.