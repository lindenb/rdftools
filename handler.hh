#ifndef RDF_HANDLER_HH
#define RDF_HANDLER_HH
#include <libxml/parser.h>
#include <libxml/tree.h>

class RDFEvent
	{
	public:
		Resource* subject;
		Resource* predicate;	
		Node object;
		int listIndex;
	};

class RDFHandler
	{
	public:
		RDFHandler();
		virtual ~RDFHandler();
		void parse(xmlTextReaderPtr r);
		void parseRDF(xmlTextReaderPtr r);
		virtual int visit(const RDFEvent* evt);
	private:
		Resource parseDescription(xmlTextReaderPtr r);
		std::auto_ptr<std::string> getAttribute(xmlTextReaderPtr r,const xmlChar * localName,const xmlChar * namespaceURI);
		std::auto_ptr<std::string> getAttribute(xmlTextReaderPtr r,const xmlChar * localName);	
	};


#endif

