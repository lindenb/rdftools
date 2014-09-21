#include "resource.hh"

using namespace std;

bool Resource::isResource() const
	{
	return true;
	}

bool Resource::isLiteral() const
	{
	return false;
	}

Node* Resource::clone() const
	{
	return new Resource(*this);
	}

bool Resource::equals(const Node* n) const
	{
	return false;
	}



const char* Resource::getURI()
	{
	return this->_uri.c_str();	
	}

Resource& Resource::operator=(const Resource& o)
	{
	if(this!=&cp)
		{
		this->_uri = o._uri;
		}
	return *this;
	}

bool Resource::operator==(const Resource& o)
	{
	return this->_uri.compare(o._uri)==0;
	}

bool Resource::operator<(const Resource& o)
	{
	return this->_uri.compare(o._uri)<0;
	}
	
bool Resource::equals(const char* ns,const char* localName)
	{
	Resource c(ns,localName);
	return this->_uri.compare(c._uri)==0;
	}	
	
	

