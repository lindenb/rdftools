#ifndef DATASTORE_HH
#define DATASTORE_HH

#include <vector>
#include <zlib.h>
#include <libxml/xmlreader.h>
#include "stmt.hh"


class DataStore
	{
	public:
		PrefixMapping prefixMapping;
		std::vector<Statement> stmts;
		
		DataStore();
		virtual ~DataStore();
		virtual void write(int fd) const;
		virtual void write(xmlTextWriterPtr w) const;
	private:
		virtual void read(int fd);
		virtual void read(xmlTextReaderPtr r);
		
	};

#endif

