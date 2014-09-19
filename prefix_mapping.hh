#ifndef PREFIX_MAPPING_H
#define PREFIX_MAPPING_H


#include<map>
#include<string>

class PrefixMapping
	{
	private:
		std::map<std::string,std::string> prefix2uri;
	public:
		PrefixMapping();
		~PrefixMapping();
		static PrefixMapping* load(int fd);
		static void write(int fd,const PrefixMapping* m);
	};

#endif

