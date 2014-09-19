

DataStore::DataStore()
	{
	
	}
DataStore::~DataStore()
	{
	
	}
DataStore& DataStore::addStatement(const Resource* S,const Resource* P,const Node* O)
	{
	Statement* stmt=Statement::createStatement(S,P,O);
	addStatement(stmt);
	delete stmt;
	return *this;
	}
	
static void x(xmlTextWriterPtr w,DataStore* ds)
	{
	xmlTextWriterStartElementNS(w, BAD_CAST RDF.NS,BAD_CAST "RDF",BAD_CAST RDF.PFX);
	DataStore::Iterator r=ds->begin();
	xmlTextWriterEndElement(w);
	}

