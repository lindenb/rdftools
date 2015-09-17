#ifndef RDF_HH
#define RDF_HH
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <redland.h>

namespace rdf {

typedef unsigned char uchar;
typedef uchar* ucharPtr;

#define WHERE(a) do { std::cerr<<"["<<__FILE__<<":" << __LINE__<<"]:" << a << std::endl; } while(0);
#define THROW(a) do {  std::ostringstream _os; _os <<"["<<__FILE__<<":" << __LINE__<<"]:" << a ; std::string _s(_os.str()); std::cerr << _s << std::endl; throw std::runtime_error(_s); }while(0)

template<typename T>
class wrapper
	{
	protected:
		T* _ptr;
	public:
		wrapper():_ptr(0)
			{
			}

		wrapper(T* t):_ptr(t)
			{
			}
		virtual  ~wrapper()
			{
			}
		T* ptr() const
			{
			if(_ptr==0) THROW("ptr is null");
			return ptr;
			}
		
		bool nil() const
			{
			return _ptr==0;
			}
	};

class URI
	{
	private:
		 librdf_uri *uri;
		 URI(librdf_uri *uri):uri(uri)
		 	{
		 	if(uri==NULL) THROW("uri is null");
		 	}
	public:
		URI():uri(0)
			{
			}
		URI(const URI& cp):uri(0)
		 	{
		 	this->uri =::librdf_new_uri_from_uri(cp.uri);
		 	if(this->uri==NULL) THROW("uri is null");
		 	}
		~URI()
			{
			WHERE(""<<(uri==0));
			::librdf_free_uri(uri);	
			WHERE("");
	 		}
		URI& operator=(const URI& cp)
			{
			if(this!=&cp)
				{
				WHERE("");
				::librdf_free_uri(uri);	
				this->uri =::librdf_new_uri_from_uri(cp.uri);
				if(this->uri==NULL) THROW("uri is null");
				}
			return *this;
			}
		
		
		bool is_file() const
			{
			return ::librdf_uri_is_file_uri(this->uri)==0;
			}
		bool  operator==(const URI& cp) const
			{
			return ::librdf_uri_equals(this->uri,cp.uri);
			}
		bool  operator<(const URI& cp) const
			{
			return std::strcmp((const char*)::librdf_uri_as_string (this->uri),(const char*)::librdf_uri_as_string (cp.uri))<0;
			}
		std::string str() const
			{
			return std::string((const char*)::librdf_uri_as_string (this->uri));
			}
	friend std::ostream& operator<<(std::ostream& out, const URI& o);
	friend class World;
	friend class ResourceUri;
	};

inline std::ostream& operator<<(std::ostream& out, const URI& o)
	{
	out << o.str();
	return out;
	}



class RDFNode
	{
	protected:
		librdf_node* _ptr;
		RDFNode(librdf_node* ptr,bool make_a_copy):_ptr(0)
			{
			if(make_a_copy && ptr!=0)
				{
				_ptr = ::librdf_new_node_from_node (ptr);
				if(_ptr == 0) THROW("Cannot copy");
				}
			else
				{
				_ptr = ptr;
				}
			}	
		
		librdf_node* ptr() const
			{
			if(_ptr==0) THROW("librdf_node is null");
			return _ptr;
			}
		
		void copy(const RDFNode* cp) 
			{
			if(this==cp) return;
				
			if(_ptr!=0) ::librdf_free_node(_ptr);
			_ptr=0;
			if(cp->_ptr!=0)
				{
				_ptr =::librdf_new_node_from_node(cp->ptr()) ;
				}
				
			}
	public:
		RDFNode():_ptr(0)
			{
			}
		
		RDFNode(const RDFNode& cp):_ptr(0)
			{
			if(cp.nil()) return;
			_ptr =  ::librdf_new_node_from_node(cp._ptr);
			if(_ptr==NULL) THROW("cannot copy");
			}
		virtual ~RDFNode()
			{
			if(_ptr!=0) ::librdf_free_node(_ptr);
			}
		virtual RDFNode* clone() const=0;
		virtual bool nil() const { return _ptr==0;}
		virtual bool is_resource() const { return ::librdf_node_is_resource(ptr());}
		virtual bool is_literal() const { return ::librdf_node_is_literal(ptr());}
		virtual bool is_blank() const { return ::librdf_node_is_blank(ptr());}
		virtual const RDFNode& print(FILE* out) const
			{
			::librdf_node_print(ptr(),out);
			return *this;
			}
		bool equals(const RDFNode* other) const
			{
			if(other==0) return false;
			return librdf_node_equals(ptr(),other->ptr());
			}
		
		virtual std::string str() const =0;
		
		
		
		virtual librdf_node_type type() const
			{
			return ::librdf_node_get_type (ptr());
			}
		friend class World;
	};
/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class Resource : public RDFNode
	{
	protected:
		Resource(librdf_node* p,bool make_a_copy):RDFNode(p,make_a_copy)
			{
			if(p==0) THROW("null ptr");
			}
	public:
		Resource()
			{
			}
		Resource(const Resource& cp):RDFNode(cp)
			{
			}
		virtual ~Resource()
			{
			}
		
		};



/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class ResourceUri : public Resource
	{
	private:
		ResourceUri(librdf_node* p,bool make_a_copy):Resource(p,make_a_copy)
			{
			}
	public:
		ResourceUri():Resource( )
			{
			}
		virtual ~ResourceUri()
			{
			}
		ResourceUri& operator=(const ResourceUri& cp)
			{
			copy(&cp);
			return *this;
			}
		URI uri() const
			{
			librdf_uri* u = ::librdf_node_get_uri (ptr());
			if(u==NULL) THROW("cannot get uri");
			 u = ::librdf_new_uri_from_uri(u);
			if(u==NULL) THROW("cannot copy uri");
			WHERE("");
			URI u2(u);
			WHERE(u2);
			return u2;
			}
		
		
		virtual RDFNode* clone() const
			{
			return new ResourceUri(*this);
			}
		
		virtual std::string str() const
			{
			WHERE("");
			return uri().str();
			}
		
	friend std::ostream& operator<<(std::ostream& out, const ResourceUri& o);
	friend class World;
	friend class Statement;
	};

inline std::ostream& operator<<(std::ostream& out, const ResourceUri& o)
	{
	out << o.str();
	return out;
	}


/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class Literal : public RDFNode
	{
	private:
		Literal(librdf_node* p,bool make_a_copy):RDFNode(p,make_a_copy)
			{
			if(p==0) THROW("null ptr");
			}
	public:
		Literal()
			{
			}
		Literal(const Literal& cp):RDFNode()
			{
			copy(&cp);
			}
		virtual ~Literal()
			{
			}
		virtual bool is_literal() const { return true;};
		Literal& operator=(const Literal& cp)
			{
			copy(&cp);
			return *this;
			}
		
		virtual RDFNode* clone() const
			{
			return new Literal(*this);
			}
		
		virtual std::string str() const
			{
			std::string s((const char*)(::librdf_node_get_literal_value(ptr())));
			return s;
			}
	friend std::ostream& operator<<(std::ostream& out, const Literal& o);
	friend class World;
	friend class Statement;
	};

inline std::ostream& operator<<(std::ostream& out, const Literal& o)
	{
	out << o.str();
	return out;
	}

class Statement
	{
	private:
		Resource* _s;
		Resource* _p;
		RDFNode* _o;
	public:
		Statement():_s(0),_p(0),_o(0)
			{
			}
		Statement(const Resource* s,const Resource* p,const RDFNode* o):
				_s((Resource*)(s==0?0:s->clone())),
				_p((Resource*)(p==0?0:p->clone())),
				_o(o==0?0:o->clone())
			{
			}
		Statement(const Statement& cp):
				_s((Resource*)(cp._s==0?0:cp._s->clone())),
				_p((Resource*)(cp._p==0?0:cp._p->clone())),
				_o(cp._o==0?0:cp._o->clone())
			{
			
			}
			
		Statement& operator=(const Statement& cp)
			{
			if(this!=&cp)
				{
				if(_s!=0) delete _s;
				if(_p!=0) delete _p;
				if(_o!=0) delete _o;
				_s = (Resource*)(cp._s==0?0:cp._s->clone());
				_p = (Resource*)(cp._p==0?0:cp._p->clone());
				_o = (cp._o==0?0:cp._o->clone());
				}
			return *this;
			}
		#define COMPARE_NODE(a) \
			if(a==NULL && cp.a!=NULL) return false;\
			if(a!=NULL && cp.a==NULL) return false;\
			if(a!=NULL && cp.a!=NULL && !(a->equals(cp.a))) return false;
			
		bool operator==(const Statement& cp) const
			{
			COMPARE_NODE(_s);
			COMPARE_NODE(_p);
			COMPARE_NODE(_o);
			return true;
			}
		#undef COMPARE_NODE
		
		virtual const Resource* subject() const  { return _s;}
		virtual const Resource* predicate() const { return _p;}
		virtual const RDFNode* object() const  { return _s;}
		
		
		
		virtual ~Statement()
			{
			if(_s!=0) delete _s;
			if(_p!=0) delete _p;
			if(_o!=0) delete _o;
			}
		virtual std::string str() const
			{
			std::ostringstream o;
			if(_s==0) { o << "(null)";} else { o << _s->str() ; }
			o << " ";
			if(_p==0) { o << "(null)";} else { o << _p->str() ; }
			o << " ";
			if(_o==0) { o << "(null)";} else { o << _o->str() ; }
			o << " .";
			return o.str();
			}

	
	friend std::ostream& operator<<(std::ostream& out, const Statement& o);
	friend class World;
	};

inline std::ostream& operator<<(std::ostream& out, const Statement& o)
	{
	out << o.str();
	return out;
	}


class Storage
	{
	private:
		librdf_storage* ptr;
		Storage(librdf_storage* ptr):ptr(ptr)
			{
			}
	public:
		
		virtual ~Storage()
			{
			::librdf_free_storage(ptr);
			}
	friend class Model;
	friend class World;
	};

class World
	{
	private:
		librdf_world* w;
	public:
		World():w(::librdf_new_world())
			{
			if(w==0) THROW("lib rdf returned null");
			::librdf_world_open(w);
			}
		virtual ~World()
			{
			::librdf_free_world(w);
			}
			
		virtual std::unique_ptr<URI> uri(const ucharPtr url)
			{
			librdf_uri *p = ::librdf_new_uri(w,url);
			if(p==0) THROW("Cannot convert uri");
			std::unique_ptr<URI> ret(new URI(p));
			return ret;
			}
		virtual std::unique_ptr<Storage> storage(const char *storage_name,
                                             const char *name,
                                             const char *options_string)
			{
			librdf_storage *p = :: librdf_new_storage(this->w,storage_name,name,options_string);
			if(p==0) THROW("Cannot convert create storage");
			std::unique_ptr<Storage> ret(new Storage(p));
			return ret;
			}
		virtual Literal literal(const ucharPtr string, const char * xml_language, int is_wf_xml) const
			{
			return Literal( ::librdf_new_node_from_literal (this->w,string,xml_language, is_wf_xml) ,false);
			}
		virtual Literal literal(const ucharPtr string) const
			{
			return literal(string,NULL,0);
			}
		virtual ResourceUri resource(const ucharPtr uri) const
			{
			return ResourceUri(::librdf_new_node_from_uri_string(this->w,uri) ,false);
			}
		/*
		virtual Statement statement(const Resource* s,const Resource* p,const RDFNode* o) const
			{
			librdf_node* sp = ::librdf_new_node_from_node (s->ptr());
			if(sp == 0) THROW("nil");
			librdf_node* pp = ::librdf_new_node_from_node (p->ptr());
			if(sp == 0) THROW("nil");
			librdf_node* op = ::librdf_new_node_from_node (o->ptr());
			if(op == 0) THROW("nil");
			librdf_statement* stmt= ::librdf_new_statement_from_nodes(
				this->w,sp,pp,op
				);
			return Statement(stmt);
			}
		*/
	friend class Storage;
	friend class URI;
	friend class Model;
	};

class Parser
	{
	private:
		 librdf_parser* parser;
	protected:
		Parser(librdf_parser* parser)
			{
			}
	public:
		virtual ~Parser()
			{
			::librdf_free_parser(parser);
			}
			
	};



class Model
	{
	public:
		Model(World* w,Storage* storage)
			{
			}
		virtual ~Model()
			{
			}
		friend class Storage;
	};


} // namespace rdf

#endif
