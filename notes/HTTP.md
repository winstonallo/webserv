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

* **_serves_** the document as requested by the client
* appears as only one machine virtually, but is often made of:
	* multiple servers sharing the load
	* other software (database server, e-commerce, ...) generating documents on demand
* you can host multiple server instances on the same machine

### proxies

between the user-agent and the server, multiple