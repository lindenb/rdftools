#ifndef RDFNS_H
#define RDFNS_H

struct Namespace
	{
	const char* NS;
	const char* PFX;
	};

extern const Namespace RDF;
extern const Namespace RDFS;
extern const Namespace DC;
extern const Namespace** DefaultNamespaces;
#endif

