#ifndef NODE_HPP
#define NODE_HPP

#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

enum node_types 
{
	UNKNOWN_NODE,
	SERVER_NODE,
	CLIENT_NODE
};

// purpose: An Parent Class that has as childs the Server and the ClientInfo.
// 			It exist as a polymorphic target for an iteration container for both of these classes. 
class Node 
{
	public:
									Node() {}
		virtual						~Node() {}
									Node(
										int fd, 
										const struct sockaddr_storage& addr,
										size_t addr_len,
										enum node_types tp
									) : 
									_fd(fd),
									_addr(addr),
									_addr_len(addr_len),
									_type(tp) {}



		int							get_fd() const { return _fd; };
		void						set_fd(int fd) { _fd = fd; }
		node_types					get_type() const { return _type; }
		void						set_type(enum node_types type) { _type = type; } 
		struct sockaddr_storage&	get_addr() { return _addr; }
		void						set_addr(const struct sockaddr_storage& address) { _addr = address; }
		size_t						get_addr_len() const { return _addr_len; }
		void						set_addr_len(size_t len) { _addr_len = len; }
		
		static void*				get_in_addr(struct sockaddr *socket_address)
		{
			if (socket_address->sa_family == AF_INET)
			{
				return &(((struct sockaddr_in *)socket_address)->sin_addr);
			}
			return &(((struct sockaddr_in6 *)socket_address)->sin6_addr);
		}

	protected:
		int							_fd;
		struct sockaddr_storage		_addr;
		size_t						_addr_len;
		enum node_types				_type;

									Node(const Node&) {}
		Node&						operator=(const Node&) { return *this; }
};

#endif
