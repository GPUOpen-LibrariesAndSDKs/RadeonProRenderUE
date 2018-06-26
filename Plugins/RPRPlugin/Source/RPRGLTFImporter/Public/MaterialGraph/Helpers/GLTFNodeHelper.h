#pragma once
#include "NameTypes.h"
#include "Platform.h"

class FGLTFNodeHelper
{
public:

	// Return the node of the name, or the index of the node if the node doesn't have name.
	template<typename TNodeType>
	static FName	GetNodeNameOfDefault(const TNodeType& Node, int32 NodeIndex);
};

template<typename TNodeType>
FName FGLTFNodeHelper::GetNodeNameOfDefault(const TNodeType& Node, int32 NodeIndex)
{
	if (Node.name.size() > 0)
	{
		return Node.name.c_str();
	}

	return *FString::FromInt(NodeIndex);
}
