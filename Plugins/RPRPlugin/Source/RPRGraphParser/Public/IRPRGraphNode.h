#pragma once
#include "SharedPointer.h"

class IRPRGraphNode
{
public:

	const FName&					GetName() const;
	virtual bool							HasChildren() const;
	virtual TArray<IRPRSchemaNodePtr>*		GetChildren() const;

protected:
	
	FName	Name;

};

typedef TSharedPtr<IRPRGraphNode> IRPRGraphNodePtr;
