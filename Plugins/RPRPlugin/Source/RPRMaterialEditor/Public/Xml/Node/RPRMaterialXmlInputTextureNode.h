#pragma once

#include "RPRMaterialXmlNode.h"
#include "Engine/Texture2D.h"
#include "SharedPointer.h"

class FRPRMaterialXmlInputTextureNode : public FRPRMaterialXmlNode
{
public:
	
	virtual ERPRMaterialNodeType GetNodeType() const override;
	
	UTexture2D*	ImportTexture(FRPRMaterialNodeSerializationContext& SerializationContext);
	

private:

	UTexture2D*	TryLoadingTextureIfAvailable(const FString& FilePath);
	void		SetDefaultRequiredTextureFormat(UTexture2D* Texture);

};

typedef TSharedPtr<FRPRMaterialXmlInputTextureNode>	FRPRMaterialXmlInputTextureNodePtr;