.PHONY:all tests
CC=g++
XML_CONFIG=xml2-config
CFLAGS= -O3 -Wall -Iinclude `$(XML_CONFIG) --cflags`
LDFLAGS= `$(XML_CONFIG) --libs`

all: tests

a.out : test.cpp 
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

tests: test/w3c/xmlbase/test013.rdf a.out
	find test/w3c -name "*.rdf" -print -exec ./a.out '{}' ';'


test/w3c/xmlbase/test013.rdf:
	rm -rf test/w3c TESTS.zip
	mkdir -p test/w3c
	wget -O TESTS.zip "http://www.w3.org/2013/RDFXMLTests/TESTS.zip"
	unzip -d  test/w3c TESTS.zip
	rm TESTS.zip
	find test/w3c -type f -name ".*.rdf" -delete

clean:
	rm -f *.o  test/w3c
