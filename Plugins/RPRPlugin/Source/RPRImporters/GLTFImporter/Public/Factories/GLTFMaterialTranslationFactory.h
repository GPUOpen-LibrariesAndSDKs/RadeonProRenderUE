//~ RPR copyright

#pragma once

#include "CoreMinimal.h"
#include "Factories/IGLTFMaterialTranslationFactory.h"
#include "GLTFTypedefs.h"
#include "GLTFMaterialTranslationFactory.generated.h"

UCLASS(hidecategories = Object)
class UGLTFMaterialTranslationFactory : public UGLTFMaterialTranslationFactoryInterface
{
    GENERATED_UCLASS_BODY()

    //~ UFactory interface

    virtual FText GetDisplayName() const override;
    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, UObject* InContext, FFeedbackContext* InWarn);

public:

    //~ UGLTFMaterialTranslationFactoryInterface interface

    bool InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial);

private:

    /** Translate material properties contained within the glTF structure to UE4 material expressions. */
    void TranslateMaterial();

    /** Create a UTexture from a texture info contained within the glTF structure. */
    UTexture* CreateTexture(int InTextureIndex);

};
