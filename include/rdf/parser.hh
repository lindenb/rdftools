#ifndef RDF_PARSER_HH
#define RDF_PARSER_HH
#include <vector>
#include <memory>
#include <libxml/parser.h>
#include "rdf/node.hh"

#define RDF_NS (BAD_CAST "http://www.w3.org/1999/02/22-rdf-syntax-ns#")
#define THROW(a) do {std::cerr <<"[" << __FILE__ << ":" << __LINE__ << "] " << a << std::endl; std::exit(-1);} while(0)
class DomParser
	{
	private:
		bool hasNs(xmlNodePtr node,const xmlChar* uri)
			{
			return node!=0 && node->ns!=0 && node->ns->href!=0 && ::xmlStrEqual(node->ns->href, uri);
			}
		bool hasLocalName(xmlNodePtr node,const char* name)
			{
			return node!=0 && node->name!=0 && ::xmlStrEqual(node->name,BAD_CAST name);
			}
		bool is_a(xmlNodePtr node,const xmlChar* uri,const char* name)
			{
			return hasNs(node,uri) && hasLocalName(node,name);
			}
	public:
		PrefixMapping prefixMapping;
		DomParser()
			{
			
			}
		
		virtual void  visit(const Statement* stmt)=0;
		
		
	private:
		void _namespaces(xmlNodePtr root)
			{
			prefixMapping.read(root);
			}
	
		void visit(const Resource* s,const Resource* p,const RDFNode* o)
			{
			Statement stmt(s,p,o);
			this->visit(&stmt);
			}
		void _checkIgnorable(xmlNodePtr root)
			{
			}
		
		bool is_anon(xmlNodePtr rsrc)
			{
			auto_xmlstr att( xmlGetNsProp(rsrc, BAD_CAST "about" , RDF_NS));
			if(!att.nil()) return false;
			att.reset(xmlGetNsProp(rsrc, BAD_CAST "ID" , RDF_NS));
			if(!att.nil()) return false;
			att.reset(xmlGetNsProp(rsrc, BAD_CAST "nodeID", RDF_NS));
			if(!att.nil()) return false;
			return true;
			};
		
		
		void parseProperty(
			xmlNodePtr property, 
			const Resource* subject,
			const char* base
			)
			{
			auto_xmlstr parseType(xmlGetNsProp(property,BAD_CAST "parseType", RDF_NS));
			auto_xmlstr dataTypeNode(xmlGetNsProp(property,BAD_CAST "dataType", RDF_NS));
			_namespaces(property);	
		
			if(property->ns == 0)
				{
				THROW( "no namespaceURI");
				}
		
			ResourceUri predicate(property);
		
			
			/** default parse type */
			if(parseType.nil())
				{
				auto_xmlstr rsrc( xmlGetNsProp(property,BAD_CAST "resource",RDF_NS));
				if(property->xmlChildrenNode==0)
					{
					if(rsrc.nil())
						{
						THROW("missing rdf:resource");
						}
					else
						{
						ResourceUri o(property->doc,property,rsrc.get());
						this->visit(subject, &predicate,&o);
						}
					}
				else
					{
					if(!rsrc.nil()) THROW("rdf:resource is present and element has children");
					if(predicate.equals(RDF_NS,BAD_CAST "type")) THROW("rdf:type expected in an empty element"); 
					int count = 0;
					xmlNodePtr firstChild = 0;
					for(xmlNodePtr n1=property->xmlChildrenNode;
						n1!=0;
						n1=n1->next)
						{
						if(n1->type!=XML_ELEMENT_NODE) continue;	
						count++;
						if(firstChild==0) firstChild=n1;
						}	
					

					switch(count)
						{
						case 0:
							{ 
							Literal L(property);
							//L.dataType= dataType;
							//L.lang= this.getLang(property);
							this->visit(subject, &predicate,&L );
							
							auto_xmlstr ID(xmlGetNsProp(property,BAD_CAST"ID",BAD_CAST RDF_NS));
							if(!ID.nil())
								{
								const ResourceUri reified(property->doc,property,ID.get());
								const ResourceUri rdf_s(RDF_NS,BAD_CAST"subject");
								const ResourceUri rdf_p(RDF_NS,BAD_CAST"predicate");
								const ResourceUri rdf_o(RDF_NS,BAD_CAST"object");
								this->visit(&reified, &rdf_s,subject);
								this->visit(&reified, &rdf_p,&predicate );
								this->visit(&reified, &rdf_o,&L );
								}
							
							break;
							}
						case 1 :
							{
							std::auto_ptr<Resource> value= this->parseResource(firstChild,base);
							this->visit(subject, &predicate,value.get());
							break;
							}
						default: THROW( "illegal number of element under.");
						}
					}
				}
			else if( parseType.equals(BAD_CAST "Literal") )
				{
				xmlBufferPtr buff = ::xmlBufferCreate();	
				for(xmlNodePtr n1=property->xmlChildrenNode;
						n1!=0;
						n1=n1->next)
						{
						::xmlNodeDump(
							buff,
							n1->doc,
							n1,
							0,0
							);
						}
				
				const xmlChar* p=::xmlBufferContent(buff);
				
				
				Literal L2(p);
				//L2.lang= this.getLang(property);
				this->visit(subject,&predicate,&L2);
					
				xmlBufferFree(buff);
				}
			else if(parseType.equals(BAD_CAST "Resource"))
				{
				const ResourceAnon rsrc;
				this->visit(subject,&predicate,&rsrc);
					
			
				for(xmlNodePtr n1=property->xmlChildrenNode;
						n1!=0;
						n1=n1->next
						)
					{
					switch(n1->type)
						{
						case XML_ELEMENT_NODE:
							{
							this->parseProperty(n1,&rsrc,base);
							break;
							}
						default: this->_checkIgnorable(n1);break;
						}
					}
			
				}
			else if(parseType.equals(BAD_CAST "Collection"))
				{
				std::vector<Resource*> list;
				for(xmlNodePtr n1=property->xmlChildrenNode;
						n1!=0;
						n1=n1->next)
					{
					switch(n1->type)
						{
						case XML_ELEMENT_NODE:
							{
							std::auto_ptr<Resource> r =	 this->parseResource(n1,base);
							list.push_back(r.release());
							break;
							}
						default: this->_checkIgnorable(n1);break;
						}
					}
			
			
				if(list.empty())
					{
					std::cerr << "Empty list" << std::endl;
					}
				else
					{
					ResourceAnon prevURI;
				
					
					this->visit(subject,&predicate,&prevURI);
					
					const ResourceUri _first(RDF_NS,BAD_CAST "first");
					const ResourceUri _rest(RDF_NS,BAD_CAST "rest");
					const ResourceUri _nil(RDF_NS,BAD_CAST "nil");
					for(std::size_t i=0;i< list.size();++i)
						{
						if(i+1==list.size())
							{
							this->visit(&prevURI,&_first, list[i]);
							this->visit(&prevURI,&_rest, &_nil);
							}
						else
							{
							ResourceAnon newURI;
							this->visit(&prevURI,&_first, list[i]);
							this->visit(&prevURI,&_rest, &newURI);
							prevURI=newURI;
							}
						delete list[i];
						}
					} 
				}
			else
				{
				THROW("illegal rdf:parseType:");
				}
			};

		
		
		std::auto_ptr<Resource> parseResource(xmlNodePtr root,const char* base)
			{
			std::auto_ptr<Resource> rsrc;
			auto_xmlstr aboutAtt(xmlGetNsProp(root,BAD_CAST"about",BAD_CAST RDF_NS));
			auto_xmlstr ID(xmlGetNsProp(root,BAD_CAST"ID",BAD_CAST RDF_NS));
			if(!aboutAtt.nil())
				{
				rsrc.reset(new ResourceUri(root->doc,root,aboutAtt.get()));
				}
			else if(!ID.nil())
				{
				rsrc.reset(new ResourceUri(root->doc,root,ID.get()));
				}
			else
				{
				rsrc.reset(new ResourceAnon());
				}
			
			
			
			if(!xmlStrEqual(root->ns->href,RDF_NS))
				{
				ResourceUri pred(RDF_NS,BAD_CAST "type");
				ResourceUri object(root);
				this->visit(rsrc.get(),&pred,&object);
				}
			else  if(xmlStrEqual(root->name,BAD_CAST "Description"))
				{
				
				}
			else
				{
				THROW("todo");
				}
			for(xmlAttrPtr attr = root->properties;
				attr !=NULL;
				attr = attr->next)
				{
				if( attr->ns!=0 && attr->ns->href!=0 && 
					xmlStrEqual(attr->ns->href,BAD_CAST "http://www.w3.org/XML/1998/namespace"))
					{
					}
				else if( attr->ns!=0 && attr->ns->href!=0 && 
					xmlStrEqual(attr->ns->href,RDF_NS))
					{
					if( xmlStrEqual(attr->name,BAD_CAST "ID") || xmlStrEqual(attr->name,BAD_CAST "about"))
						{
						}
					else
						{
						std::cerr << "ignore att @rdf:" << (const char*)attr->name << std::endl;
						}
					//ignore
					}
				else
					{
					ResourceUri pred(attr->ns->href,attr->name);
					auto_xmlstr attvalue(::xmlNodeListGetString(root->doc, attr->children, 1));
					Literal object(attvalue.get());
					this->visit(rsrc.get(),&pred,&object);
					}
				}
			
			for(xmlNodePtr c = root->xmlChildrenNode; c!=NULL;c=c->next)
				{
				switch(c->type)
					{
					case XML_ELEMENT_NODE : this->parseProperty(c,rsrc.get(),base); break;
					default: this->_checkIgnorable(c);break;
					}
				}
			return rsrc;
			}
			
	public:
		void parse(xmlDocPtr dom,const char* base)
			{
			if(dom==0) THROW("dom is null");
			xmlNodePtr	root = xmlDocGetRootElement(dom);
			if(root==0) THROW("root is null");
			if(!is_a(root,RDF_NS, "RDF")) THROW("not a RDF root");
			_namespaces(root);
			
			for(xmlNodePtr c = root->xmlChildrenNode; c!=NULL;c=c->next)
				{
				if(c->type==XML_ELEMENT_NODE)
					{
					std::auto_ptr<Resource> r= this->parseResource(c,base);
					}
				else
					{
					_checkIgnorable(c);
					}
				}
			}
	};
	
#undef RDF_NS

#endif
