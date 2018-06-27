//~ RPR copyright

#pragma once

#include "RPRMaterialGLTFNode.h"
#include "Templates/SharedPointer.h"
#include "RPRMaterialGraphSerializationContext.h"

class UTexture2D;

class FRPRMaterialGLTFImageTextureNode : public FRPRMaterialGLTFNode
{
public:

    virtual RPRMaterialGLTF::ERPRMaterialNodeType GetNodeType() const override;

    UTexture2D* ImportTexture(FRPRMaterialGraphSerializationContext& SerializationContext);

private:

    UTexture2D* TryLoadingTextureIfAvailable(const FString& FilePath);

    void SetDefaultRequiredTextureFormat(UTexture2D* Texture);

};

typedef TSharedPtr<FRPRMaterialGLTFImageTextureNode> FRPRMaterialGLTFImageTextureNodePtr;
