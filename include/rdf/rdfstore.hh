#ifndef RDF_STORE_HH
#define RDF_STORE_HH
#include <set>
#include "rdf/parser.hh"

class AbstractStore
	{

	public:
		AbstractStore()
			{
			}
		virtual ~AbstractStore()
			{
			}
		virtual bool add(const Statement stmt)=0;
	protected:	
		class my_parser : public DomParser
			{
			public:
				AbstractStore* delegate;
				my_parser(AbstractStore* delegate):delegate(delegate)
					{
					}
				
				void  visit(const Statement* stmt)
					{
					delegate->add(*stmt);
					}
			};
	};

class DefaultStore : public AbstractStore, public std::set<Statement>
	{
	public:
		DefaultStore()
			{
			}
		
		virtual bool add(const Statement stmt)
			{
			this->insert(stmt);
			return true;
			}
		DefaultStore& read(const char* filename)
			{
			xmlDocPtr	dom = ::xmlParseFile(filename);
			if( dom == NULL) throw std::runtime_error("failed to read");
			my_parser parser(this);
			parser.parse(dom,filename);
			::xmlFreeDoc(dom);
			return *this;
			}
		xmlDocPtr toDom()
			{
			xmlDocPtr res;
   			xmlNsPtr rdf;
			res = xmlNewDoc("1.0");
			res->xmlRootNode = xmlNewDocNode(res, NULL, "RDF", NULL);
			rdf = xmlNewNs(res->xmlRootNode, "http://www.w3.org/TR/WD-rdf-syntax#", "RDF");
			rdf = xmlNewNs(res->xmlRootNode, "http://www.w3.org/1999/02/22-rdf-syntax-ns#", "rdf");
			xmlSetNs(res->xmlRootNode, rdf);
			std::set<Statement>::const_iterator r=begin();
			std::set<Statement>::const_iterator r_end=end();
			while(r!=r_end)
				{
				std::set<Statement>::const_iterator r_next=r;
				xmlNodePtr subjetNode = ::xmlNewNode();
				if(r->subject()->is_anon())
					{
					}
				else
					{
					::xmlSetNsProp(
						subjetNode,
						
						RDF_NS,
						BAD_CAST "about"
						);
					}
				while(r_next!=r_end && 
					  r->subject()->equals(r_next->subject()))
					{
					
					++r_next;
					}
				
				xmlAddChild(res->xmlRootNode,subjetNode);
				r=r_next;
				}
			return res;
			}
		
		DefaultStore& n3(std::ostream& out)
			{
			std::set<Statement>::const_iterator r=begin();
			std::set<Statement>::const_iterator r_end=end();
			while(r!=r_end)
				{
				r->n3(out);
				++r;
				}
			return *this;
			}
	};

#endif

