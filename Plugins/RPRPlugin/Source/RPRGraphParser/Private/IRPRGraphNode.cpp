#include "IRPRGraphNode.h"

const FName& IRPRGraphNode::GetName() const
{
	return (Name);
}

bool IRPRGraphNode::HasChildren() const
{
	return (false);
}

TArray<IRPRSchemaNodePtr>* IRPRGraphNode::GetChildren() const
{
	return (nullptr);
}
