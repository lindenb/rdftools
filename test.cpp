#include "rdf.hh"

using namespace std;

using namespace rdf;

int main(int argc,char** argv)
	{
	World w;
	URI u(&w,"http://google.com");
	cout << u << endl;
	ResourceUri x(&w,"http://google.com");
	ResourceUri p(&w,"http://predicate.com");
	cout << x << endl;
	Literal m(&w,"Hello");
	cout << m << endl;
	Literal n(&w,2);
	cout << n << endl;
	Statement stmt(&w,&x,&p,&n);
	cout << stmt << endl;
	FileModel rdfFile(&w,"jeter.rdf");
	for(int i=0;i< 10;++i)
		{
		Literal n3(&w,i);
		Statement stmt2(&w,x,p,n3);
		rdfFile.add(stmt2);
		rdfFile.add("http://wikipedia.org", "http://x","http://google.com");
		}
	return 0;
	}
