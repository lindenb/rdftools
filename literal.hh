class Literal: public Node
	{
	private:
		const xsd* xsdType; 
		Literal();
		Literal(const Literal& cp);
	public:
		virtual ~Literal();
		virtual bool isResource() const ;
		virtual bool isLiteral() const ;
		virtual Node* clone() const;
		virtual bool equals(const Node* n) const;
		static Literal* createLiteral(int i);
		static Literal* createLiteral(const char*);
	};
