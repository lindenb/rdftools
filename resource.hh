#ifndef RSRC_HH
#define RSRC_HH

#include <string>
#include "node.hh"

class Resource: public Node
	{
	private:
		std::string _uri;
	public:
		Resource();
		Resource(const Resource& cp);
		virtual ~Resource();
		bool isResource() const ;
		bool isLiteral() const ;
		Node* clone() const;
		bool equals(const Node* n) const;
	};

#endif

