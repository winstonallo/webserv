#ifndef NODE_HPP
#define NODE_HPP

#include <iostream>
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

// purpose: An Parent Class that has as childs the ServerInfo and the ClientInfo.
// 			It exist as a polymorphic target for an iteration container for both of these classes. 
class Node 
{
	public:
									Node();
									~Node();
									Node(int fd, 
										const struct sockaddr_storage& addr,
										size_t addr_len,
										enum node_types tp);
									Node(const Node& rhs);
		Node&						operator=(const Node& rhs);


		int							get_fd() const;
		void						set_fd(int tfd);
		node_types					get_type() const;
		void						set_type(enum node_types t);
		struct sockaddr_storage		get_addr() const;
		void						set_addr(const struct sockaddr_storage& val);
		size_t						get_addr_len() const;
		void						set_addr_len(size_t len);
		static void*				get_in_addr(struct sockaddr *sa);

	protected:
		int							fd;
		struct sockaddr_storage		addr;
		size_t						addr_len;
		enum node_types				type;
};

std::ostream& operator<<(std::ostream& os, const Node& nd);

#endif
