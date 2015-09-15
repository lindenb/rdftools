#ifndef RDF_XML_HH
#define RDF_XML_HH
#include <string>
#include <iostream>
#include <libxml/tree.h>
#include <libxml/uri.h>

typedef  std::basic_string<xmlChar> xml_string;



class auto_xmlstr
	{
	private:
		xmlChar* ptr;
	public:
		auto_xmlstr(xmlChar* s):ptr(s)
			{
			}
		~auto_xmlstr()
			{
			if(ptr!=0) ::xmlFree(ptr);
			}
		bool nil() const { return ptr==NULL;}
		xmlChar* get() { return ptr;}
		bool equals(const xmlChar* s) const
			{
			if(s==0 || ptr==0) return false;
			return (bool)::xmlStrEqual(s,ptr);
			}
		bool equals(const auto_xmlstr& cp) const
			{
			return this->equals(cp.ptr);
			}
		auto_xmlstr& reset(xmlChar* s)
			{
			if(s!=ptr)
				{
				if(ptr!=0) ::xmlFree(ptr);
				ptr=s;
				}
			return *this;
			}
		std::ostream& print(std::ostream& out) const
			{
			if( ptr==0)
				{
				out << "(NULL)";
				}
			else
				{
				out << (const char*)ptr;
				}
			return out;
			}
	};



#endif

