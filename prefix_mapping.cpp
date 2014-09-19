#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include "prefix_mapping.hh"
#include "ns.hh"

using namespace std;



PrefixMapping::PrefixMapping()
	{
	size_t i=0;
	while(DefaultNamespaces[i]!=0)
		{
		prefix2uri.insert(make_pair<string,string>(
			DefaultNamespaces[i]->PFX,
			DefaultNamespaces[i]->NS
			));
		++i;
		}
	}
	
PrefixMapping::~PrefixMapping()
	{
	}
	
PrefixMapping* PrefixMapping::load(int fd)
	{
	PrefixMapping* pm=new PrefixMapping;
	map<std::string,std::string>::size_type n;
	read(fd, (void*)&n,sizeof(n));
	for(size_t i=0;i< n;++i)
		{
		size_t j;
		read(fd, (void*)&j,sizeof(j));
		string pfx(j,'\0');
		read(fd, (void*)pfx.data(),j);
		
		read(fd, (void*)&j,sizeof(j));
		string uri(j,'\0');
		read(fd, (void*)uri.data(),j);
		
		pm->prefix2uri.insert(make_pair<string,string>(pfx,uri));
		}
	return pm;
	}
#define SAFE_WRITE(fd,ptr,n) if(::write(fd,(void*)ptr,n)!=n) exit(-1);
#define WRITE_STRING(fd,s) \
	i= s.size();\
	SAFE_WRITE(fd,&i,sizeof(i));\
	SAFE_WRITE(fd,s.data(),i)

void PrefixMapping::write(int fd,const PrefixMapping* m)
	{
	map<std::string,std::string>::size_type i= m->prefix2uri.size();
	::write(fd,(void*)&i,sizeof(i));
	map<std::string,std::string>::const_iterator r= m->prefix2uri.begin();
	while(r!= m->prefix2uri.end())
		{
		WRITE_STRING(fd,r->first);
		WRITE_STRING(fd,r->second);
		++r;
		}
	}
