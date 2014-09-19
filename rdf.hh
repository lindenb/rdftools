#ifndef RDF_HH
#define RDF_HH
#include<map>
#include<string>
#include<vector>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h> 

#include "ns.hh"
#include "prefix_mapping.hh"








class StmtFilter
	{
	private:
		Resource* _sub;
		Resource* _pred;
		Node* _obj;

	public:
		StmtFilter(const Resource* S,const Resource* P,const Node* O);
		~StmtFilter();
	};





#endif

