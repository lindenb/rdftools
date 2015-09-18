SHELL=/bin/bash
this.makefile=$(lastword $(MAKEFILE_LIST))
this.dir=$(dir $(realpath ${this.makefile}))

.PHONY:all tests redland.libs
CC=g++
XML_CONFIG=xml2-config
CFLAGS= -g -std=c++0x -O3 -Wall -Isrc `ext/redland/bin/redland-config --cflags`
LDFLAGS= `ext/redland/bin/redland-config --libs`

all: a.out

a.out : test.cpp redland.libs src/rdf.cpp src/rdf.hh
	$(CC) -o $@ $(CFLAGS) $(filter %.cpp,$^) $(LDFLAGS)

raptor.version=2.0.15

ext/raptor2/bin/rapper :
	mkdir -p ext/raptor2
	rm -rf ext/raptor2/raptor2-${raptor.version} ext/raptor2/raptor2-${raptor.version}.tar.gz
	wget -O ext/raptor2/raptor2-${raptor.version}.tar.gz "http://download.librdf.org/source/raptor2-${raptor.version}.tar.gz"
	(cd ext/raptor2 && tar xvfz raptor2-${raptor.version}.tar.gz && cd raptor2-${raptor.version} && ./configure --prefix=$${PWD}/.. && make && make install)
	rm ext/raptor2/raptor2-${raptor.version}.tar.gz

rasqal.version=0.9.33
ext/rasqal/bin/roqet : ext/raptor2/bin/rapper
	mkdir -p ext/rasqal
	rm -rf ext/rasqal/rasqal-${rasqal.version} ext/rasqal/rasqal-${rasqal.version}.tar.gz
	wget -O ext/rasqal/rasqal-${rasqal.version}.tar.gz "http://download.librdf.org/source/rasqal-${rasqal.version}.tar.gz"
	(cd ext/rasqal && tar xvfz rasqal-${rasqal.version}.tar.gz && cd rasqal-${rasqal.version} && PKG_CONFIG_PATH=../../raptor2/lib/pkgconfig RAPTOR2_CFLAGS=" -I${this.dir}ext/raptor2/include -I${this.dir}ext/raptor2/include/raptor2 " RAPTOR2_LIBS=" -L${this.dir}ext/raptor2/lib -lraptor2 " ./configure --prefix=${this.dir}/ext/rasqal && make && make install)
	rm ext/rasqal/rasqal-${rasqal.version}.tar.gz


redland.version=1.0.16
ext/redland/bin/redland-config : ext/rasqal/bin/roqet
	mkdir -p ext/redland
	rm -rf ext/redland/redland-${redland.version} ext/redland/redland-${redland.version}.tar.gz
	wget -O ext/redland/redland-${redland.version}.tar.gz "http://download.librdf.org/source/redland-${redland.version}.tar.gz"
	(cd ext/redland && tar xvfz redland-${redland.version}.tar.gz && cd redland-${redland.version} && PKG_CONFIG_PATH=${this.dir}ext/raptor2/lib/pkgconfig:${this.dir}ext/rasqal/lib/pkgconfig RAPTOR2_CFLAGS=" -I${this.dir}ext/raptor2/include -I${this.dir}ext/raptor2/include/raptor2 " RAPTOR2_LIBS=" -L${this.dir}ext/raptor2/lib -lraptor2 " ./configure --prefix=${this.dir}/ext/redland && make && make install)
	rm ext/redland/redland-${redland.version}.tar.gz

redland.libs: ext/redland/bin/rdfproc 

clean:
	rm -f *.o  ext


