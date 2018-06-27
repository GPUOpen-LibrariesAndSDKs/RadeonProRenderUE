#pragma once
#include "INodeParamType.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialNode.h"
#include "UnrealType.h"
#include "SharedPointer.h"

template<typename TNode>
class FNodeParamType : public INodeParamType
{

public:

	void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext, 
		IRPRMaterialNodePtr CurrentNode, 
		UProperty* Property) override
	{
		TSharedPtr<TNode> currentNodePtr = StaticCastSharedPtr<TNode>(CurrentNode);
		LoadRPRMaterialParameters(SerializationContext, currentNodePtr, Property);
	}

protected:

	virtual void LoadRPRMaterialParameters(
		FRPRMaterialGraphSerializationContext& SerializationContext,
		TSharedPtr<TNode> CurrentNode,
		UProperty* Property) = 0;

};