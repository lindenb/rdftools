#ifndef RDF_NODE_HH
#define RDF_NODE_HH
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include "rdf/prefixmapping.hh"


class RDFNode
	{
	public:
		RDFNode() {}
		virtual ~RDFNode(){}
		virtual bool is_resource() const=0;
		virtual bool is_anon() const=0;
		virtual bool is_literal() const=0;
		virtual int compare(const RDFNode* cp) const=0;
		virtual RDFNode* clone() const =0;
		virtual std::ostream& n3(std::ostream& out) const=0;
		virtual bool equals(const RDFNode* cp) const
			{
			return compare(cp)==0;
			}
	};

class Resource : public RDFNode
	{
	public:
		Resource() {}
		virtual ~Resource(){}
		virtual bool is_resource() const { return true;}
		virtual bool is_literal() const { return false;}
		virtual bool is_anon() const=0;
		virtual RDFNode* clone() const =0;
		virtual std::ostream& n3(std::ostream& out) const=0;
	};

class ResourceUri : public Resource
	{
	private:
		xml_string _uri;
	public:
		ResourceUri(const xmlChar* _uri):_uri(_uri) {}
		ResourceUri(xml_string _uri):_uri(_uri) {}
		ResourceUri(const xmlChar* ns,const xmlChar* local):_uri(ns) {_uri.append(local);}
		ResourceUri(const xmlChar* ns,size_t n):_uri(ns,n) {}
		ResourceUri(const ResourceUri& cp):_uri(cp._uri) {}
		ResourceUri(xmlNodePtr root)
			{
			if(root->ns==0) throw std::runtime_error("ns is null");
			 _uri.assign(root->ns->href);
			 _uri.append(root->name);
			}
		ResourceUri(xmlDocPtr dom,xmlNodePtr root,const xmlChar* uri)
			{
			auto_xmlstr xmlbase(::xmlNodeGetBase( root->doc,root));
			if(!xmlbase.nil())
				{
				auto_xmlstr resolved(::xmlBuildURI(uri,xmlbase.get()));
				if(resolved.nil()) throw std::runtime_error("resolved is null");
				_uri.assign( resolved.get() );
				}
			else
				{
				std::cerr << "XML BASE IS NILL" << std::endl;
				_uri.assign( uri);
				}
			}
		bool equals(const xmlChar* ns,const xmlChar* local)
			{
			xml_string s(ns);
			s.append(local);
			return _uri.compare(s)==0;
			}
		virtual ~ResourceUri(){}
		virtual bool is_anon() const { return false;}
		virtual bool is_resource() const { return true;}
		virtual bool is_literal() const { return false;}
		virtual const xml_string uri() const { return this->_uri;} 
		virtual RDFNode* clone() const
			{
			return new ResourceUri(this->uri());
			}
		virtual int compare(const RDFNode* cp) const
			{
			if(!cp->is_resource()) return -1;
			if(cp->is_anon()) return -1;
			const ResourceUri* r=(const ResourceUri*)cp;
			return this->uri().compare(r->uri());
			}
		virtual std::ostream& n3(std::ostream& out) const
			{
			out << "<" << _uri.c_str() << ">";
			return out;
			}
		virtual const xmlChar* localName() const
			{
			std::size_t h1=_uri.find_last_of('#');
			std::size_t h2=_uri.find_last_of('/');
			if(h1==xml_string::npos && h2==xml_string::npos)
				{
				return 0;
				}
			return &(_uri.c_str()[std::max(h1,h2)]);
			}
	};

class ResourceAnon : public Resource
	{
	private:
		long _uri;
	public:
		ResourceAnon()
			{
			static long id_generator = 0L;
			this->_uri=id_generator++;
			}
		ResourceAnon(long _uri):_uri(_uri) {}
		ResourceAnon(const ResourceAnon& cp):_uri(cp._uri) {}
		virtual ~ResourceAnon(){}
		virtual bool is_anon() const { return true;}
		virtual bool is_resource() const { return true;}
		virtual bool is_literal() const { return false;}
		virtual long uri() const { return this->_uri;} 
		virtual RDFNode* clone() const
			{
			return new ResourceAnon(this->uri());
			}
		virtual int compare(const RDFNode* cp) const
			{
			if(!cp->is_resource()) return -1;
			if(!cp->is_anon()) return 1;
			const ResourceAnon* r=(const ResourceAnon*)cp;
			return this->uri()==(r->uri());
			}
		virtual std::ostream& n3(std::ostream& out) const
			{
			out << "_:j" << _uri;
			return out;
			}
	};



class Literal : public RDFNode
	{
	private:
		std::string lang;
		std::string dataType;
		xml_string value;
	public:
		Literal(xml_string value):value(value) {}
		Literal(xmlNode * cur)
			{
			auto_xmlstr c(xmlNodeGetContent(cur));
			value.assign(c.get());
			}
		virtual ~Literal(){}
		virtual bool is_anon() const { return false;}
		virtual bool is_resource() const { return false;}
		virtual bool is_literal() const { return true;}
		virtual RDFNode* clone() const
			{
			return new Literal(this->value);
			}
		virtual int compare(const RDFNode* cp) const
			{
			if(!cp->is_literal()) return 1;
			const Literal* r=(const Literal*)cp;
			return this->value.compare(r->value);
			}
		virtual std::ostream& n3(std::ostream& out) const
			{
			out << "\"" << value.c_str() << "\"";
			return out;
			}
	};

class Statement
	{
	private:
		Resource* s;
		Resource* p;
		RDFNode* o;
	public:
		Statement(const Resource* s,const Resource* p,const RDFNode* o):
			s((Resource*)s->clone()),
			p((Resource*)p->clone()),
			o(o->clone())
			{
			}
		Statement(const Statement& cp):
			s((Resource*)cp.s->clone()),
			p((Resource*)cp.p->clone()),
			o(cp.o->clone())
			{
			}
		~Statement()
			{
			delete s;
			delete p;
			delete o;
			}
		Statement& operator=(const Statement& cp)
			{
			if(this!=&cp)
				{
				delete s;
				delete p;
				delete o;
				s= (Resource*)cp.s->clone();
				p= (Resource*)cp.p->clone();
				o= cp.o->clone();
				}
			return *this;
			}
		bool operator==(const Statement& cp) const
			{
			int i= s->compare(cp.s);
			if(i!=0) return false;
			i= p->compare(cp.p);
			if(i!=0) return false;
			i= o->compare(cp.o);
			if(i!=0) return false;
			return true;
			}
		bool operator<(const Statement& cp) const
			{
			int i= s->compare(cp.s);
			if(i<0) return true;
			i= p->compare(cp.p);
			if(i<0) return true;
			i= o->compare(cp.o);
			if(i<0) return true;
			return false;
			}
		virtual std::ostream& n3(std::ostream& out) const
			{
			s->n3(out);
			out << " ";
			p->n3(out);
			out << " ";
			o->n3(out);
			out << " ." << std::endl;
			return out;
			}
		const Resource* subject() const { return s;}
		const Resource* predicate() const {  return p;}
		const RDFNode* object()  const { return o;}
	};

#endif

