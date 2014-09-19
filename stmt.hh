class Statement
	{
	private:
		Resource* _sub;
		Resource* _pred;
		Node* _obj;

	public:
		Statement(const Resource& S,const Resource& P,const Node& O);
		~Statement();
		const Resource& subject() const;
		const Resource& predicate() const;
		const Node& object() const;
		bool equals(const Statement* stmt) const;
		Statement& operator=(const Statement& cp);
		bool operator==(const Statement& cp) const;
	};


