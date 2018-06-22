#pragma once
#include "IRPRGraphSchema.h"

class FRPRMaterialXmlSchema : public IRPRGraphSchema<FXmlNode>
{

public:

	FRPRMaterialXmlSchema();
	
	virtual bool NextNode(const FXmlNode& ElementType, IRPRSchemaNodePtr& OutNode) override;

private:

	IRPRSchemaNodePtr	CreateNode(const FXmlNode* XmlNode) const;

private:

	int32 CurrentNodeIndex;

};