#include "rdf.hh"

using namespace std;

using namespace rdf;

int main(int argc,char** argv)
	{
	URI u("http://google.com");
	cout << u << endl;
	ResourceUri x("http://google.com");
	ResourceUri p("http://predicate.com");
	cout << x << endl;
	Literal m("Hello");
	cout << m << endl;
	Literal n(2);
	cout << n << endl;
	Statement stmt(&x,&p,&n);
	cout << stmt << endl;
	FileModel rdfFile("jeter.rdf");
	for(int i=0;i< 10;++i)
		{
		Literal n3(i);
		Statement stmt2(x,p,n3);
		rdfFile.add(stmt2);
		rdfFile.add("http://wikipedia.org", "http://x","http://google.com");
		}
	return 0;
	}
