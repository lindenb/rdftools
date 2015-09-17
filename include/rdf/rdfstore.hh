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
		PrefixMapping prefixMapping;
		DefaultStore()
			{
			}
		
		virtual bool add(const Statement stmt)
			{
			this->insert(stmt);
			return true;
			}
		DefaultStore& readFile(const char* filename)
			{
			xmlDocPtr	dom = ::xmlParseFile(filename);
			if( dom == NULL) throw std::runtime_error("failed to read");
			my_parser parser(this);
			parser.parse(dom,filename);
			prefixMapping.merge(parser.prefixMapping);
			::xmlFreeDoc(dom);
			return *this;
			}
			
		xmlDocPtr toDom()
			{
			
			xmlDocPtr res = 0;
			
   			xmlNsPtr rdf;
			res = xmlNewDoc(BAD_CAST"1.0");
			res->xmlRootNode = xmlNewDocNode(res, NULL,BAD_CAST "RDF", NULL);
			std::vector<xmlNsPtr> nsptrs;
			xmlNsPtr rdfNs=NULL;
			std::set<xml_string> prefixes = this->prefixMapping.prefixes();
			for(std::set<xml_string>::const_iterator r=prefixes.begin();
				r!=prefixes.end();
				++r)
				{
				xmlNsPtr ns = xmlNewNs(res->xmlRootNode,prefixMapping.getUri(r->c_str()), r->c_str());
				nsptrs.push_back(ns);
				if( r->compare(BAD_CAST "http://www.w3.org/1999/02/22-rdf-syntax-ns#")==0)
					{
					rdfNs=ns;
					::xmlSetNs(res->xmlRootNode, rdfNs);
					}
				}
			
			

			std::set<Statement>::const_iterator r=begin();
			std::set<Statement>::const_iterator r_end=end();
			while(r!=r_end)
				{
				std::set<Statement>::const_iterator r_next=r;
				xmlNodePtr subjectNode=xmlNewNode(
						rdfNs,
						BAD_CAST "rdf:Description"
						);
					
				if(r->subject()->is_anon())
					{
				
					}
				else
					{
					xml_string uri( ((ResourceUri*)r->subject())->uri() );
					::xmlSetNsProp(
						 subjectNode, 
						 rdfNs, 
						 BAD_CAST "rdf:about", 
						 ((ResourceUri*)r->subject())->uri().c_str()
						 );
					}
				
				while(r_next!=r_end && 
					  r->subject()->equals(r_next->subject()))
					{
					
					++r_next;
					}
				
				xmlAddChild(res->xmlRootNode,subjectNode);
				r++;
				}
				
			return res;
			}
		DefaultStore xml(FILE* out)
			{
			xmlDocPtr dom = this->toDom();
			::xmlDocDump(out,dom);
			::xmlFreeDoc(dom);
			return *this;
			}
			
		DefaultStore& n3(std::ostream& out)
			{
			std::set<xml_string> prefixes = this->prefixMapping.prefixes();
			for(std::set<xml_string>::const_iterator r=prefixes.begin();
				r!=prefixes.end();
				++r)
				{
				out << "PREFIX " << r->c_str() << " "<< prefixMapping.getUri(r->c_str()) << "> ." << std::endl;
				}
			
			std::set<Statement>::const_iterator r=this->begin();
			std::set<Statement>::const_iterator r_end=this->end();
			while(r!=r_end)
				{
				r->n3(out);
				++r;
				}
			return *this;
			}
	};

#endif

