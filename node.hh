#ifndef RDFNODE_H
#define RDFNODE_H

class Node
	{
	protected:
		Node();
	public:
		Node(const Node& cp);
		virtual ~Node();
		virtual bool isResource() const =0;
		virtual bool isLiteral()const =0;
		virtual Node* clone() const =0;
		virtual bool equals(const Node* n) const=0;
		
	};



#endif

