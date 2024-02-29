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
* **Authentication**