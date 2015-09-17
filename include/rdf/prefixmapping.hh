#ifndef RDF_PREFIXMAPPING_HH
#define RDF_PREFIXMAPPING_HH
#include <map>
#include <set>
#include "rdf/xml.hh"

class PrefixMapping
	{
	private:
		std::map<xml_string,xml_string> prefix2uri;
	public:
		
		PrefixMapping()
			{
			prefix2uri.insert(std::make_pair<xml_string,xml_string>(BAD_CAST "rdf",BAD_CAST "http://www.w3.org/1999/02/22-rdf-syntax-ns#"));
			}
		PrefixMapping(const PrefixMapping& cp):prefix2uri(cp.prefix2uri)
			{
			}
		
		std::set<xml_string> prefixes() const
			{
			std::set<xml_string> prefixes;
			for(std::map<xml_string,xml_string>::const_iterator r=prefix2uri.begin();
				r!=prefix2uri.end();
				++r)
				{
				prefixes.insert(r->first);
				}
			return prefixes;
			}
		
		const xmlChar* getUri(const xmlChar* prefix) const
			{
			std::map<xml_string,xml_string>::const_iterator r = prefix2uri.find(prefix);
			if(r==prefix2uri.end()) return 0;
			return r->second.c_str();
			}
		const xmlChar* getPrefix(const xmlChar* uri) const
			{
			if( uri == NULL) return NULL;
			for(std::map<xml_string,xml_string>::const_iterator r=prefix2uri.begin();
				r!=prefix2uri.end();
				++r)
				{
				if( ::xmlStrEqual(r->second.c_str(),uri)) return r->first.c_str();
				}
			return NULL;
			}
		
		
		
		
		
		PrefixMapping& operator=(const PrefixMapping& cp)
			{
			if(this!=&cp)
				{
				prefix2uri=cp.prefix2uri;
				}
			return *this;
			}
			
		PrefixMapping& merge(const PrefixMapping& cp)
			{
			if(this==&cp) return *this;
			for(std::map<xml_string,xml_string>::const_iterator r=cp.prefix2uri.begin();
				r!=cp.prefix2uri.end();
				++r)
				{
				if(getUri(r->first.c_str())!=NULL) continue;
				if(getPrefix(r->second.c_str())!=NULL) continue;
				prefix2uri.insert(std::make_pair<xml_string,xml_string>(r->first,r->second));
				}
			return *this;
			}
		
		PrefixMapping& read(xmlNodePtr root)
			{
			xmlNsPtr* nsList = ::xmlGetNsList(root->doc, root);
			if(nsList==NULL) return *this;
			for(int i=0;nsList[i]!=NULL;++i)
				{
				xmlNsPtr ns= nsList[i];
				if(ns->prefix==NULL || ns->prefix[0]==0) { ns=ns->next; continue;}
				if(getUri(ns->prefix)!=NULL) continue;
				prefix2uri.insert(std::make_pair<xml_string,xml_string>(BAD_CAST ns->prefix,BAD_CAST ns->href));
				}
			return *this;
			}
		
	};


#endif

