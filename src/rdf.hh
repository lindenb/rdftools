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
typedef const ucharPtr cucharPtr;

#define WHERE(a) do { std::cerr<<"["<<__FILE__<<":" << __LINE__<<"]:" << a << std::endl; } while(0);
#define THROW(a) do {  std::ostringstream _os; _os <<"["<<__FILE__<<":" << __LINE__<<"]:" << a ; std::string _s(_os.str()); std::cerr << _s << std::endl; throw std::runtime_error(_s); }while(0)



template<typename T>
class wrapper
	{
	protected:
		T* _ptr;
		void reset(T* p)
			{
			this->_ptr=p;
			}
	public:
		wrapper():_ptr(0)
			{
			}

		wrapper(T* t):_ptr(t)
			{
			}
		
		wrapper(T* t,bool nilleable):_ptr(t)
			{
			if(t==0 && !nilleable)
				{
				THROW("Failure, ptr initialized with NULL");
				}
			}
		
		virtual  ~wrapper()
			{
			}
		
		T* get() const
			{
			return _ptr;
			}
		
		T* ptr() const
			{
			if(get()==0) THROW("ptr is null");
			return get();
			}
		
		
		bool nil() const
			{
			return get()==0;
			}
	};
	

class Storage;
class URI;
class Model;
class ResourceUri;
class Literal;

class World: public wrapper<librdf_world>
	{
	public:
		World();
		virtual ~World();
		virtual URI uri(const ucharPtr url);
		virtual Literal literal(const ucharPtr string, const char * xml_language, int is_wf_xml) const;
		virtual Literal literal(const ucharPtr string) const;
		virtual ResourceUri resource(const ucharPtr uri) const;
		static World* instance();
	friend class Storage;
	friend class URI;
	friend class Model;
	private:
		static World* INSTANCE;
	};




class URI: public wrapper<librdf_uri>
	{
	private:
		 URI(librdf_uri *uri);
	public:

		URI(const ucharPtr url);
		URI(const char* url);
		
		URI(const World* w,const ucharPtr url);
		URI();
		URI(const URI& cp);
		virtual ~URI();
		URI& operator=(const URI& cp);
		
		bool is_file() const;
		bool  operator==(const URI& cp) const;
		bool  operator<(const URI& cp) const;
		std::string str() const;
	friend std::ostream& operator<<(std::ostream& out, const URI& o);
	friend class World;
	friend class ResourceUri;
	};

inline std::ostream& operator<<(std::ostream& out, const URI& o)
	{
	out << o.str();
	return out;
	}



class RDFNode: public wrapper<librdf_node>
	{
	protected:
		RDFNode(librdf_node* ptr,bool make_a_copy);
		void copy(const RDFNode* cp) ;
		RDFNode();
	public:
		RDFNode(const RDFNode& cp);
		virtual ~RDFNode();
		virtual RDFNode* clone() const=0;
		virtual bool is_resource() const;
		virtual bool is_literal() const;
		virtual bool is_blank() const;
		virtual const RDFNode& print(FILE* out) const;
		bool equals(const RDFNode* other) const;
		virtual std::string str() const =0;
		virtual librdf_node_type type() const;
		friend class World;
	};

/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class Resource : public RDFNode
	{
	protected:
		Resource(librdf_node* p,bool make_a_copy);
		Resource();
	public:
		Resource(const Resource& cp);
		virtual ~Resource();
		};



/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class ResourceUri : public Resource
	{
	private:
		ResourceUri(librdf_node* p,bool make_a_copy);
	public:
		ResourceUri(const World* w,const char* uri);
		ResourceUri(const World* w,cucharPtr uri);
		ResourceUri(const char* uri);
		ResourceUri(cucharPtr uri);
		ResourceUri();
		virtual ~ResourceUri();
		virtual ResourceUri& operator=(const ResourceUri& cp);
		virtual URI uri() const;
		virtual RDFNode* clone() const;
		virtual std::string str() const;
		
	friend std::ostream& operator<<(std::ostream& out, const ResourceUri& o);
	friend class World;
	friend class Statement;
	};

inline std::ostream& operator<<(std::ostream& out, const ResourceUri& o)
	{
	out << o.str();
	return out;
	}

#define DECLARE_LITERAL(TYPE) \
		Literal(const World* w,TYPE data);\
		Literal(TYPE data)

/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class Literal : public RDFNode
	{
	private:
		Literal(librdf_node* p,bool make_a_copy);
	public:
		Literal();
		Literal(const World* w,const char* text);
		Literal(const World* w,cucharPtr text);
		Literal(const char* text);
		Literal(cucharPtr text);
		DECLARE_LITERAL(double);
		DECLARE_LITERAL(float);
		DECLARE_LITERAL(int);
		DECLARE_LITERAL(short);
		DECLARE_LITERAL(long);
		DECLARE_LITERAL(bool);
		
		
		Literal(const Literal& cp);
		virtual ~Literal();
		virtual bool is_literal() const ;
		Literal& operator=(const Literal& cp);
		virtual RDFNode* clone() const;
		virtual std::string str() const;
	friend std::ostream& operator<<(std::ostream& out, const Literal& o);
	friend class World;
	friend class Statement;
	};
#undef DECLARE_LITERAL

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
		Statement();
		Statement(const Resource* s,const Resource* p,const RDFNode* o);
		Statement(const Resource& s,const Resource& p,const RDFNode& o);
		Statement(const Statement& cp);
		Statement& operator=(const Statement& cp);
		bool operator==(const Statement& cp) const;
		
		virtual const Resource* subject() const ;
		virtual const Resource* predicate() const;
		virtual const RDFNode* object() const;
		

		
		virtual ~Statement();
		virtual std::string str() const;

	
	friend std::ostream& operator<<(std::ostream& out, const Statement& o);
	friend class World;
	};

inline std::ostream& operator<<(std::ostream& out, const Statement& o)
	{
	out << o.str();
	return out;
	}


class Storage: public wrapper<librdf_storage>
	{	

		
	public:
		
		virtual void open() =0;
		virtual void close()
			{
			if(!nil()) ::librdf_free_storage(ptr());
			reset(0);
			}
		virtual ~Storage()
			{
			close();
			}
	friend class Model;
	friend class World;
	};

class FileStorage: public Storage
	{	
	private:
		std::string filename;
	public:
		FileStorage(const char* filename):Storage()
			{
			}
		virtual void open()
			{
			close();
			reset(::librdf_new_storage(World::instance()->ptr(), "file", filename.c_str(), NULL));
			ptr();
			}
		virtual ~FileStorage()
			{
			}
	friend class Model;
	friend class World;
	};


class Parser: public wrapper< librdf_parser>
	{
	protected:
		Parser(librdf_parser* ptr):wrapper(ptr,false)
			{
			}
	public:
		virtual ~Parser()
			{
			::librdf_free_parser(ptr());
			}
			
	};
class StmtIterator: public wrapper<librdf_stream>
	{
	public:
		StmtIterator():wrapper( librdf_new_empty_stream(World::instance()->ptr()),false);
			{
			}
		virtual ~StmtIterator()
			{
			::librdf_free_stream (ptr());
			}
		bool hasNext() const
			{
			return ::librdf_stream_end (ptr());
			}
		Statement next() const
			{
			if( librdf_stream_next (ptr())!=0) THROW("cannot move");
			return ::librdf_stream_end (ptr());
			}
	};


class Model : public wrapper<librdf_model>
	{
	protected:
		librdf_storage* storage;
	
		Model():wrapper(0,true),storage(0)
			{
			}
		
		librdf_node* safe_copy(const RDFNode* c) const
			{
			if(c==0) THROW("nil node");
			librdf_node* x=::librdf_new_node_from_node(c->ptr());
			if(x==0) THROW("Cannot copy node");
		 	return x;
			}	
		
		librdf_statement* to_statement(const Statement* stmt) const
		 	{
		 	 librdf_statement* x= ::librdf_new_statement_from_nodes(World::instance()->ptr(),
		 		safe_copy(stmt->subject()),
		 		safe_copy(stmt->predicate()),
		 		safe_copy(stmt->object())
		 		); 
		 	if(x==0) THROW("Cannot convert to ibrdf_statement");
		 	return x;
		 	}
	public:
		virtual ~Model()
			{
			if(!nil()) librdf_free_model(ptr());
			if(storage!=0) librdf_free_storage(storage);
			}
		int size() const
			{
			return ::librdf_model_size(ptr());
			}
		virtual void add(const Statement& stmt)
			{
			librdf_statement *statement = this->to_statement(&stmt);
			int ret= ::librdf_model_add_statement(ptr(), statement);
			librdf_free_statement(statement);
			if(ret!=0) THROW("Cannot add " << stmt );
			}
		
		virtual void remove(const Statement& stmt)
			{
			librdf_statement *statement = this->to_statement(&stmt);
			int ret= ::librdf_model_remove_statement(ptr(), statement);
			librdf_free_statement(statement);
			if(ret!=0) THROW("Cannot add " << stmt );
			}
		
		bool contains(const Statement& stmt)
			{
			librdf_statement *statement = this->to_statement(&stmt);
			bool ret= ::librdf_model_contains_statement(ptr(), statement);
			librdf_free_statement(statement);
			return ret;
			}
		
		virtual void add(const char* s, const char* p, const char* o)
			{
			ResourceUri _s(s);
			ResourceUri _p(p);
			if(strstr(o,"://")==NULL)
				{
				Literal _o(o);
				Statement stmt(_s,_p,_o);
				add(stmt);
				}
			else
				{
				ResourceUri _o(o);
				Statement stmt(_s,_p,_o);
				add(stmt);
				}
			}
		
	};



class FileModel : public Model
	{
	private:
		std::string filename;
	public:
		FileModel(const char* filename):Model(),filename(filename)
			{
			this->storage=librdf_new_storage(World::instance()->ptr(), "file", filename, NULL);
			reset(::librdf_new_model(World::instance()->ptr(), storage, NULL));
			ptr();
			}
		virtual ~FileModel()
			{

			}
		
	};



} // namespace rdf

#endif
