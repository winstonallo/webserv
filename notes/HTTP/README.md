# HTTP

**_Hypertext Transfer Protocol:_** protocol for fetching resources like as HTML docs
*	client-server protcol -> requests initiated by recipient (eg. web browser)

client & servers communicate by exchanging messages
*	client message: **request**
*	server message: **response**

## components

### user-agent (client)
	
* user-agent -> tool acting on behalf of the user
	* web-browser
	* debugging software
* **always** initiates the request

to display a Web page, the browser:
* sends an original request to fetch the HTML document
* parses file
* makes additional requests based on 
	* execution scripts
	* layout info (CSS)
	* sub-resources (images, videos, ...)
* combines the resources to present the complete document, _the Web page_

a Web page is a hypertext document, meaning parts of the content are links which can be activated to fetch a new page, allowing the user to direct their user-agent

* server
	* handle request
	* provide answer (response)
* proxies
	* act as gateways
	* perform different operations

### the Web server
.
* **_serves_** the document as requested by the client
* appears as only one machine virtually, but is often made of:
	* multiple servers sharing the load
	* other software (database server, e-commerce, ...) generating documents on demand
* you can host multiple server instances on the same machine

### proxies

between the user-agent and the server, multiple machines work on relaying the HTTP messages
due to the layers of the Web stack, most of these operate at the
* transport level,
* network level or
* physical level,
& they become transparent on the HTTP layer.
the ones operating at application layer are called **proxies**.
proxies can be:
* **_transparent:_** (just forwarding requests) or
* **_non-transparent:_** change the request before passing it along, eg by:
	* caching
	* filtering
	* load balancing
	* authentication
	* logging

## basic aspects

HTTP is designed to be:
* human-readable for:
	* easier testing
	* newcomer access
* extensible -> headers allow to change functionality easily
* stateless - no link between 2 separate requests
	* coherent access is managed through HTTP cookies, allowing different requests to share the same context

a connection is controlled at the transport layer -> **_therefore out of scope for HTTP_**, HTTP does not need the transport protocol to be connection-based, it needs it to be **_reliable_**, as in **_not lose messages_** (or reporting in error in such cases)
for that reason, HTTP relies on TCP - which is connection-based

before a client and a server can exchange an HTTP request/response pair, they need a TCP connection
the default behavior in HTTP/1.0 is opening a separate TCP connection for each pair (which is slow when you spam requests)

HTTP/1.1 introduced **_pipelining_** and **_persistent connections_**: the TCP connection can be controlled using the ```Connection``` header

## what can HTTP control

common features controllable with HTTP:
* **caching**
	* the server can:
		* tell proxies/clients what to cache
	* the client can:
		* make proxies ignore stored documents
* **relaxing origin constraint**
	* Web browsers enforce strict separation between websites - only pages from the same origin can access all information of a Web page
	-> HTTP headers can relax this separation
* **authentication**
	* HTTP supports authentication, using:  for example:
		* the ```WWW-Authenticate``` header
		* setting a user-specific session using cookies
* **proxy and tunneling**
	* HTTP requests are able to go through proxies to cross network barriers
* **sessions**
	* HTTP cookies allow to link requests with the server state, creating sessions - this is useful for:
		* e-commerce (shopping baskets)
		* sites allowing user config

## HTTP flow

### client-server communication steps

1. **open a TCP connection - the client may:**
	* open a new connection
	* reuse an existing connection
	* open several conncetions
2. **send a HTTP message**
	```HTTP
	GET / HTTP/1.1
	Host: developer.mozilla.org 
	Accept-Language: fr
	```
3. **read server response**
	```HTTP
	HTTP/1.1 200 OK
	Date: Sat, 09 Oct 2010 14:28:02 GMT
	Server: Apache
	Last-Modified: Tue, 01 Dec 2009 20:18:22 GMT
	ETag: "51142bc1-7449-479b075b2891b"
	Accept-Ranges: bytes
	Content-Length: 29769
	Content-Type: text/html
	<!DOCTYPE html>… (here come the 29769 bytes of the requested web page)
	```
4. **close or keep the connection for later requests**

## HTTP messages

HTTP messages (until HTTP/1.1) are human-readable.

there are two types of HTTP messages, **_requests and responses_**, each has its own format

### requests

**requests consist of the following elements:**
* HTTP method, for example:
	* ``GET``
	* ``POST``
	* ``OPTIONS``
	* ``HEAD``
		* _in most cases, the client wants to ``GET`` a resource or ``POST`` the value of an HTML form_
* the path to the resource - some URL, **without info that is obvious from the context**, like:
	* the protocol (duh)
	* the TCP port
* the version of the HTTP protocol
* optional: 
	* optional headers
	* a body, for some methods like ``POST``, which contains the resource sent

### responses

**responses consist of the following elements**
* the version of the HTTP protocol
* a ``status code``, (was the request successful?)
* a status message, shortly describing the status code
* HTTP ``headers``
* optional: 
	* a body containing the fetched resource
