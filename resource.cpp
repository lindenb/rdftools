#include "resource.hh"


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


