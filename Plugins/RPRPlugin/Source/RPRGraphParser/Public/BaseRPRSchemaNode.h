#pragma once

template<typename TNodeType>
class FBaseRPRSchemaNode : public IRPRSchemaNode
{
protected:

	FName		Name;
	TNodeType	Type;

};
