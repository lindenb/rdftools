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
		Resource(const char* s);
		Resource(const char* ns,const char* localName);
		Resource(const Resource& cp);
		virtual ~Resource();
		bool isResource() const ;
		bool isLiteral() const ;
		Node* clone() const;
		bool equals(const Node* n) const;
		const char* getURI() const;
		bool equals(const char* ns,const char* localName);
		bool operator==(const Resource& cp) const;
		bool operator<(const Resource& cp) const;
		Resource& operator=(const  Resource& r);
		
	};

#endif

