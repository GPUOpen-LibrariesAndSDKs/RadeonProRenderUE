#pragma once
#include "SharedPointer.h"
#include "RPRMaterialNode.h"

class INodeParamType
{
public:

	virtual void	LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext,
								IRPRMaterialNodePtr CurrentNode,
								class UProperty* Property) = 0;

};

using INodeParamTypePtr = TSharedPtr<INodeParamType>;