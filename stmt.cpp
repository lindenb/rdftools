Statement::Statement(const Resource* S,const Resource* P,const Node* O):
	_sub((Resource*)S->clone()),
	_pred((Resource*)P->clone()),
	 _obj((Resource*)O->clone())
	{
	}

Statement::~Statement()
	{
	}

Statement*
Statement::createStatement(const Resource* S,const Resource* P,const Node* O)
	{
	return new Statement(S,P,O);
	}

