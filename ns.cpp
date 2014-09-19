#include "ns.hh"

const Namespace RDF={"http://www.w3.org/1999/02/22-rdf-syntax-ns#","rdf"};
const Namespace RDFS={"http://www.w3.org/2000/01/rdf-schema#","rdfs"};
const Namespace DC={"http://purl.org/dc/elements/1.1/","dc"};


extern const Namespace* DefaultNamespaces={
	RDF,RDFS,DC,0
	};
