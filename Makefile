.PHONY:all
CC=g++
XML_CONFIG=xml2-config
CFLAGS= -O3 -Wall `$(XML_CONFIG) --cflags`
LDFLAGS= `$(XML_CONFIG) --libs`

all: 

rdftool: rdftool.o
	$(CC) -o $@ $(CFLAGS) $^ $(LDFLAGS)

node.o: node.cpp node.hh
	$(CC) -o $@ -c $(CFLAGS) $<	

prefix_mapping.o : prefix_mapping.cpp prefix_mapping.hh ns.hh
	$(CC) -o $@ -c $(CFLAGS) $<

ns.o : ns.cpp ns.hh
	$(CC) -o $@ -c $(CFLAGS) $<




rdftool.o : rdftool.cpp
	$(CC) -o $@ -c $(CFLAGS) $<

rdf.o : rdf.cpp rdf.hh
	$(CC) -o $@ -c $(CFLAGS) $<

clean:
	rm -f *.o 
