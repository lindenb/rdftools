#include "literal.hh"

Literal::Literal():Node()
	{
	}

Literal::Literal(const Literal& cp):Node()
	{
	
	}

Literal::~Literal()
	{
	}

bool Literal::isResource() const
	{
	return false;
	}
bool Literal::isLiteral() const
	{
	return true;
	}

Node* Literal::clone() const
	{
	return new Literal(*this);
	}

bool Literal::equals(const Node* n) const
	{
	return false;
	}

Literal* Literal::createLiteral(int i)
	{
	Literal* L=new Literal();
	return L;
	}

Literal* Literal::createLiteral(const char*)
	{
	Literal* L=new Literal();
	return L;
	}

