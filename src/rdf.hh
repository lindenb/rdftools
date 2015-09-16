#ifndef RDF_HH
#define RDF_HH

class RDFNode
	{
	protected:
		librdf_node* ptr;
		RDFNode(librdf_node* ptr):ptr(ptr)
			{
			}
		void copy(const RDFNode* cp) 
			{
			if(this==cp) return;
				
			if(ptr!=0) ::librdf_free_node(ptr);
			ptr=0;
			if(cp->ptr!=0)
				{
				ptr=::librdf_new_node_from_node(cp->ptr) ;
				}
				
			}
	public:
		RDFNode():ptr(0)
			{
			}
		
		RDFNode(const RDFNode& cp):ptr(0)
			{
			if(cp.nil()) return;
			ptr =  ::librdf_new_node_from_node(cp.ptr);
			}
		virtual ~RDFNode()
			{
			if(ptr!=0) ::librdf_free_node(ptr);
			}
		virtual RDFNode* clone() const=0;
		virtual bool nil() const { return ptr==0;}
		virtual bool is_resource() const { return !nil() && ::librdf_node_is_resource(ptr));
		virtual bool is_literal() const { return !nil() && ::librdf_node_is_literal(ptr));
		virtual bool is_blank() const { return !nil() && ::librdf_node_is_blank(ptr));
		virtual RDFNode& print(FILE* out) const
			{
			::librdf_node_print(ptr,out);
			}
		
		
		virtual std::string str() const
		   {
		   std::string result("(null)");
		   if (!nil())
		    {
			char *s = (char *)::librdf_node_to_string(ptr) ;
			result = std::string(s) ;
			std::free(s) ;
			}
		  return result ;
		  }
		
		
		
		virtual librdf_node_type type() const
			{
			return ::librdf_node_get_type (ptr);
			}
	};
/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class Resource : public RDFNode
	{
	public:
		Resource()
			{
			}
		virtual ~Resource()
			{
			}
		virtual bool is_resource() const { return !nil()};
	};



/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class ResourceUri : public Resource
	{
	public:
		ResourceUri()
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
	};


/* http://iconocla.st/hacks/query/redland-0.9.15/docs/api/ */
class Literal : public RDFNode
	{
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
	};


#endif
