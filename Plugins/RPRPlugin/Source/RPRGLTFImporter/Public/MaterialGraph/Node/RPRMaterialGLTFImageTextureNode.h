//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"

class UTexture2D;

class FRPRMaterialGLTFImageTextureNode : public FRPRMaterialGLTFNode
{
public:

    virtual ERPRMaterialNodeType GetNodeType() const override;

    UTexture2D* ImportTexture(FRPRMaterialGLTFSerializationContext& SerializationContext);

private:

    UTexture2D* TryLoadingTextureIfAvailable(const FString& FilePath);

    void SetDefaultRequiredTextureFormat(UTexture2D* Texture);

};

typedef TSharedPtr<FRPRMaterialGLTFImageTextureNode> FRPRMaterialGLTFImageTextureNodePtr;
