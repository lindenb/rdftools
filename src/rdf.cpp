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


	
	
void URI::init(const World* w,const ucharPtr url)
	{
	VERIFY(w!=0);
	librdf_uri* p =  ::librdf_new_uri(w->ptr(),url);
	VERIFY(p!=0);
	reset(p);
	reset_world(w);
	}

URI::URI(const World* w,librdf_uri* p):world_wrapper(w,p)
	{
	VERIFY(p!=0);
	}

URI::URI(const World* w,const ucharPtr url):world_wrapper()
	{
	init(w,url);
	}

URI::URI(const World* w,const char* url):world_wrapper()
	{
	init(w,(const ucharPtr)url);
	}

URI::URI():world_wrapper()
	{
	}
URI::URI(const URI& cp):world_wrapper(cp.world(),0)
 	{
 	if(!cp.nil())
 		{
		librdf_uri* p =	::librdf_new_uri_from_uri(cp.ptr());
		VERIFY(p!=0);
		reset(p);
		}
 	}
 
URI::~URI()
	{
	if(!nil()) ::librdf_free_uri(ptr());	
	}
URI& URI::operator=(const URI& cp)
	{
	if(this!=&cp)
		{
		if(!nil()) ::librdf_free_uri(ptr());	
		reset(0);
		if(!cp.nil())
			{
			librdf_uri* p =	::librdf_new_uri_from_uri(cp.ptr());
			VERIFY(p!=0);
			reset(p);
			}
		reset_world(cp.world());
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

/********************************************************************/


RDFNode::RDFNode(const World* w,librdf_node* ptr,bool make_a_copy):world_wrapper(w,0,true)
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

RDFNode::RDFNode():world_wrapper(0,0,true)
	{
	}

RDFNode::RDFNode(const RDFNode& cp):world_wrapper(cp.world(),0,true)
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




Resource::Resource(const World* w,librdf_node* p,bool make_a_copy):RDFNode(w,p,make_a_copy)
	{
	if(p==0) THROW("null ptr");
	}



Resource::Resource(const World* w):RDFNode(w,0,true)
	{
	}
Resource::Resource(const Resource& cp):RDFNode(cp)
	{
	}
Resource::~Resource()
	{
	}
		



ResourceUri::ResourceUri(const World* w,const char* uri):Resource(w,::librdf_new_node_from_uri_string(w->ptr(),(cucharPtr)uri) ,false)
	{
	ptr();
	}
ResourceUri::ResourceUri(const World* w,cucharPtr uri):Resource(w, ::librdf_new_node_from_uri_string(w->ptr(),uri) ,false)
	{
	ptr();
	}


ResourceUri::ResourceUri():Resource(0)
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
	URI u2(World::instance(),u);
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





Literal::Literal(const World* w,const char* text):RDFNode(w,::librdf_new_node_from_literal (w->ptr(),(cucharPtr)text,NULL, 0) ,false)
	{
	}
Literal::Literal(const World* w,cucharPtr text):RDFNode(w,::librdf_new_node_from_literal (w->ptr(),text,NULL, 0) ,false)
	{
	ptr();
	}


#define DECLARE_LITERAL(TYPE,XSD) \
Literal::Literal(const World* w,TYPE data):RDFNode(w,0,true) \
	{ \
	URI uri(w,(ucharPtr)"http://www.w3.org/2001/XMLSchema#" XSD);\
	std::ostringstream os; os << data; std::string s(os.str()); \
	reset(::librdf_new_node_from_typed_literal(w->ptr(),(cucharPtr)s.c_str(),NULL,uri.ptr())); \
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


Statement::Statement():_w(0),_s(0),_p(0),_o(0)
			{
			}
Statement::Statement(const World* w,const Resource* s,const Resource* p,const RDFNode* o):
				_w((World*)w),
				_s((Resource*)(s==0?0:s->clone())),
				_p((Resource*)(p==0?0:p->clone())),
				_o(o==0?0:o->clone())
			{
			WHERE( (_s->str()) << ";" << (_p->str()) << ";" << (_o->str()));
			}
Statement::Statement(const World* w,const Resource& s,const Resource& p,const RDFNode& o):
				_w((World*)w),
				_s((Resource*)s.clone()),
				_p((Resource*)p.clone()),
				_o(o.clone())
			{
			}
Statement::Statement(const Statement& cp):
				_w((World*)cp._w),
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
				_w = cp._w;
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


