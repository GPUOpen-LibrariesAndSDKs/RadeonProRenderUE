#pragma once

#include "RPRMaterialXmlNode.h"
#include "SharedPointer.h"

class FRPRMaterialXmlUberNode : public FRPRMaterialXmlNode
{
public:

	virtual ERPRMaterialNodeType GetNodeType() const override;

	void	Serialize(FRPRMaterialNodeSerializationContext& SerializationContext);

};

typedef TSharedPtr<FRPRMaterialXmlUberNode>	FRPRMaterialXmlUberNodePtr;