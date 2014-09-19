#ifndef DATASTORE_HH
#define DATASTORE_HH

#include <vector>
#include "stmt.hh"


class DataStore
	{
	public:
		PrefixMapping prefixMapping;
		std::vector<Statement> stmts;
		
		DataStore();
		virtual ~DataStore();
		
	};

#endif

