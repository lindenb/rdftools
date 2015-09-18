#include "rdf.hh"

using namespace std;
using namespace rdf;

World* World::INSTANCE = 0;

World*  World::instance()
	{
	if( World::INSTANCE == 0 )
		{
		World::INSTANCE = new World();
		}
	return World::INSTANCE;
	}


World::World(): wrapper<librdf_world>(::librdf_new_world(),false)
	{
	::librdf_world_open(ptr());
	}
World::~World()
	{
	::librdf_free_world(ptr());
	}

		
URI World::uri(const ucharPtr url)
	{
	return URI (this,url);
	}
 Literal World::literal(const ucharPtr string, const char * xml_language, int is_wf_xml) const
	{
	return Literal( ::librdf_new_node_from_literal (this->ptr(),string,xml_language, is_wf_xml) ,false);
	}
 Literal World::literal(const ucharPtr string) const
	{
	return literal(string,NULL,0);
	}
 ResourceUri World::resource(const ucharPtr uri) const
	{
	return ResourceUri(this,uri);
	}
	
	
	
URI::URI(librdf_uri* ptr):wrapper(ptr,false)
	{
	}

URI::URI(const ucharPtr url):wrapper( ::librdf_new_uri(World::instance()->ptr(),url),false)
	{
	}

URI::URI(const char* url):wrapper( ::librdf_new_uri(World::instance()->ptr(),(cucharPtr)url),false)
	{
	}

URI::URI(const World* w,const ucharPtr url):wrapper( ::librdf_new_uri(w->ptr(),url),false)
	{
	}
URI::URI():wrapper(0,true)
	{
	}
URI::URI(const URI& cp):wrapper(::librdf_new_uri_from_uri(cp.ptr()),false)
 	{
 	}
URI::~URI()
	{
	if(!nil()) ::librdf_free_uri(ptr());	
	}
URI& URI::operator=(const URI& cp)
	{
	if(this!=&cp)
		{
		::librdf_free_uri(ptr());	
		reset(::librdf_new_uri_from_uri(cp.ptr()));
		ptr();
		}
	return *this;
	}

bool URI::is_file() const
	{
	return !nil() && ::librdf_uri_is_file_uri(ptr())==0;
	}
bool  URI::operator==(const URI& cp) const
	{
	return ::librdf_uri_equals(this->ptr(),cp.ptr());
	}
bool URI::operator<(const URI& cp) const
	{
	return std::strcmp((const char*)::librdf_uri_as_string (this->ptr()),(const char*)::librdf_uri_as_string (cp.ptr()))<0;
	}
std::string URI::str() const
	{
	return std::string((const char*)::librdf_uri_as_string (this->ptr()));
	}



RDFNode::RDFNode(librdf_node* ptr,bool make_a_copy):wrapper(0,true)
	{
	if(make_a_copy && ptr!=0)
		{
		reset(  ::librdf_new_node_from_node (ptr));
		if(nil()) THROW("Cannot copy");
		}
	else
		{
		reset(ptr);
		}
	}	
		

void RDFNode::copy(const RDFNode* cp) 
	{
	if(this==cp) return;
		
	if(!nil()) ::librdf_free_node(ptr());
	reset(0);
	if(!cp->nil())
		{
		reset(::librdf_new_node_from_node(cp->ptr()));
		}
		
	}

RDFNode::RDFNode():wrapper(0,true)
	{
	}

RDFNode::RDFNode(const RDFNode& cp):wrapper(0,true)
	{
	if(cp.nil()) return;
	reset(::librdf_new_node_from_node(cp.ptr()));
	if(_ptr==NULL) THROW("cannot copy");
	}
RDFNode::~RDFNode()
	{
	if(!nil()) ::librdf_free_node(ptr());
	}

 bool RDFNode::is_resource() const { return ::librdf_node_is_resource(ptr());}
 bool RDFNode::is_literal() const { return ::librdf_node_is_literal(ptr());}
 bool RDFNode::is_blank() const { return ::librdf_node_is_blank(ptr());}
 const RDFNode& RDFNode::print(FILE* out) const
	{
	::librdf_node_print(ptr(),out);
	return *this;
	}
bool RDFNode::equals(const RDFNode* other) const
	{
	if(other==0) return false;
	return librdf_node_equals(ptr(),other->ptr());
	}


 librdf_node_type RDFNode::type() const
	{
	return ::librdf_node_get_type (ptr());
	}




Resource::Resource(librdf_node* p,bool make_a_copy):RDFNode(p,make_a_copy)
	{
	if(p==0) THROW("null ptr");
	}



Resource::Resource()
	{
	}
Resource::Resource(const Resource& cp):RDFNode(cp)
	{
	}
Resource::~Resource()
	{
	}
		

ResourceUri::ResourceUri(const char* uri):Resource( ::librdf_new_node_from_uri_string(World::instance()->ptr(),(cucharPtr)uri) ,false)
	{
	ptr();
	}
ResourceUri::ResourceUri(cucharPtr uri):Resource( ::librdf_new_node_from_uri_string(World::instance()->ptr(),uri) ,false)
	{
	ptr();
	}

ResourceUri::ResourceUri(const World* w,const char* uri):Resource( ::librdf_new_node_from_uri_string(w->ptr(),(cucharPtr)uri) ,false)
	{
	ptr();
	}
ResourceUri::ResourceUri(const World* w,cucharPtr uri):Resource( ::librdf_new_node_from_uri_string(w->ptr(),uri) ,false)
	{
	ptr();
	}


ResourceUri::ResourceUri(librdf_node* p,bool make_a_copy):Resource(p,make_a_copy)
			{
			}

ResourceUri::ResourceUri():Resource( )
			{
			}
ResourceUri::~ResourceUri()
			{
			}
ResourceUri& ResourceUri::operator=(const ResourceUri& cp)
	{
	copy(&cp);
	return *this;
	}
URI ResourceUri::uri() const
	{
	if(nil()) THROW("Nil URI");
	librdf_uri* u = ::librdf_node_get_uri (ptr());
	if(u==NULL) THROW("cannot get uri from "<< get());
	 u = ::librdf_new_uri_from_uri(u);
	if(u==NULL) THROW("cannot copy uri");
	URI u2(u);
	return u2;
	}


RDFNode* ResourceUri::clone() const
	{
	return new ResourceUri(*this);
	}

std::string ResourceUri::str() const
	{
	return uri().str();
	}




Literal::Literal(librdf_node* p,bool make_a_copy):RDFNode(p,make_a_copy)
	{
	if(p==0) THROW("null ptr");
	}

Literal::Literal(const World* w,const char* text):RDFNode(::librdf_new_node_from_literal (w->ptr(),(cucharPtr)text,NULL, 0) ,false)
	{
	}
Literal::Literal(const World* w,cucharPtr text):RDFNode(::librdf_new_node_from_literal (w->ptr(),text,NULL, 0) ,false)
	{
	ptr();
	}
Literal::Literal(const char* text):RDFNode(::librdf_new_node_from_literal (World::instance()->ptr(),(cucharPtr)text,NULL, 0) ,false)
	{
	ptr();
	}
Literal::Literal(cucharPtr text):RDFNode(::librdf_new_node_from_literal (World::instance()->ptr(),text,NULL, 0) ,false)
	{
	ptr();
	}


#define DECLARE_LITERAL(TYPE,XSD) \
Literal::Literal(const World* w,TYPE data):RDFNode() \
	{ \
	URI uri(w,(ucharPtr)"http://www.w3.org/2001/XMLSchema#" XSD);\
	std::ostringstream os; os << data; std::string s(os.str()); \
	reset(::librdf_new_node_from_typed_literal(w->ptr(),(cucharPtr)s.c_str(),NULL,uri.ptr())); \
	ptr(); \
	} \
Literal::Literal(TYPE data):RDFNode() \
	{ \
	URI uri(World::instance(),(ucharPtr)"http://www.w3.org/2001/XMLSchema#" XSD);\
	std::ostringstream os; os << data; std::string s(os.str()); \
	reset(::librdf_new_node_from_typed_literal(World::instance()->ptr(),(cucharPtr)s.c_str(),NULL, uri.ptr())); \
	ptr(); \
	}
DECLARE_LITERAL(double,"double")
DECLARE_LITERAL(float,"float")
DECLARE_LITERAL(int,"int")
DECLARE_LITERAL(short,"short")
DECLARE_LITERAL(long,"long")
DECLARE_LITERAL(bool,"bool")

#undef  DECLARE_LITERAL

Literal::Literal()
			{
			}
Literal::Literal(const Literal& cp):RDFNode()
			{
			copy(&cp);
			}
Literal::~Literal()
			{
			}

bool Literal::is_literal() const { return true;};
Literal& Literal::operator=(const Literal& cp)
	{
	copy(&cp);
	return *this;
	}

 RDFNode* Literal::clone() const
	{
	return new Literal(*this);
	}

 std::string Literal::str() const
	{
	std::string s((const char*)(::librdf_node_get_literal_value(ptr())));
	return s;
	}


Statement::Statement():_s(0),_p(0),_o(0)
			{
			}
Statement::Statement(const Resource* s,const Resource* p,const RDFNode* o):
				_s((Resource*)(s==0?0:s->clone())),
				_p((Resource*)(p==0?0:p->clone())),
				_o(o==0?0:o->clone())
			{
			WHERE( (_s->str()) << ";" << (_p->str()) << ";" << (_o->str()));
			}
Statement::Statement(const Resource& s,const Resource& p,const RDFNode& o):
				_s((Resource*)s.clone()),
				_p((Resource*)p.clone()),
				_o(o.clone())
			{
			}
Statement::Statement(const Statement& cp):
				_s((Resource*)(cp._s==0?0:cp._s->clone())),
				_p((Resource*)(cp._p==0?0:cp._p->clone())),
				_o(cp._o==0?0:cp._o->clone())
			{
			WHERE( (_s->str()) << ";" << (_p->str()) << ";" << (_o->str()));
			}
			
		Statement& Statement::operator=(const Statement& cp)
			{
			if(this!=&cp)
				{
				if(_s!=0) delete _s;
				if(_p!=0) delete _p;
				if(_o!=0) delete _o;
				_s = (Resource*)(cp._s==0?0:cp._s->clone());
				_p = (Resource*)(cp._p==0?0:cp._p->clone());
				_o = (cp._o==0?0:cp._o->clone());
				WHERE( (_s->str()) << ";" << (_p->str()) << ";" << (_o->str()));
				}
			return *this;
			}
		#define COMPARE_NODE(a) \
			if(a==NULL && cp.a!=NULL) return false;\
			if(a!=NULL && cp.a==NULL) return false;\
			if(a!=NULL && cp.a!=NULL && !(a->equals(cp.a))) return false;
			
		bool Statement::operator==(const Statement& cp) const
			{
			COMPARE_NODE(_s);
			COMPARE_NODE(_p);
			COMPARE_NODE(_o);
			return true;
			}
		#undef COMPARE_NODE
		
		 const Resource* Statement::subject() const  { return _s;}
		 const Resource* Statement::predicate() const { return _p;}
		 const RDFNode* Statement::object() const  { return _o;}
		
		
		
		 Statement::~Statement()
			{
			if(_s!=0) delete _s;
			if(_p!=0) delete _p;
			if(_o!=0) delete _o;
			}
		 std::string Statement::str() const
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


