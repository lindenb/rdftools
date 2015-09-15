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
			}
		PrefixMapping(const PrefixMapping& cp):prefix2uri(cp.prefix2uri)
			{
			}
		
		std::set<xml_string> prefixes()
			{
			}
		
	};


#endif

