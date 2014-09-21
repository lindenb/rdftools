#include "datastore.hh"

DataStore::DataStore()
	{
	
	}
DataStore::~DataStore()
	{
	
	}

static const char magic[4]={'R','D','F','\1'};

struct version_t
	{
	int major;
	int minor;
	int micro;
	};
	
	
static const version version_t file_version={1,0,0};

enum opcode_t={
	OP_START_SUBJECT,
	OP_END_SUBJECT,
	OP_PREDICATE_OBJECT,
	OP_STMT,
	OP_EOF
	};

void DataStore::write(int w) const
	{
	int opcode=0;
	size_t i=0;
	//save magic
	::write(w,(const void*)&magic, sizeof(magic));
	//save version
	::write(w,(const void*)&file_version, sizeof(struct version_t));
	
	while(i < this->stmts.size())
		{
		size_t j=i+1;
		while(	j <  this->stmts.size() && 
			this->stmts.at(i).subject() == this->stmts.at(j).subject()
			)
			{
			++j;
			}
		//consecutive subjects
		if( i + 1 < j)
			{
			opcode=OP_START_SUBJECT;
			::write(w,(const void*)&opcode, sizeof(opcode));
			write(this->stmts.at(i).subject());
			while(i<j)
				{
				opcode=OP_PREDICATE_OBJECT;
				::write(w,(const void*)&opcode, sizeof(opcode));
				write(this->stmts.at(i).predicate());
				write(this->stmts.at(i).object());
				++i;
				}
			opcode=OP_END_SUBJECT;
			::write(w,(const void*)&opcode, sizeof(opcode));
			}
		else
			{
			
			}
		i=j;
		}	
	opcode=OP_EOF;
	::write(w,(const void*)&opcode, sizeof(opcode));
	}
