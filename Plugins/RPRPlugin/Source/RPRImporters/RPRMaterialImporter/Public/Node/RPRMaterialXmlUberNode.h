#pragma once

#include "RPRMaterialXmlNode.h"
#include "SharedPointer.h"

class RPRMATERIALIMPORTER_API FRPRMaterialXmlUberNode : public FRPRMaterialXmlNode
{
public:

	virtual RPRMaterialXml::ERPRMaterialNodeType GetNodeType() const override;

	void	LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext);

};

typedef TSharedPtr<FRPRMaterialXmlUberNode>	FRPRMaterialXmlUberNodePtr;