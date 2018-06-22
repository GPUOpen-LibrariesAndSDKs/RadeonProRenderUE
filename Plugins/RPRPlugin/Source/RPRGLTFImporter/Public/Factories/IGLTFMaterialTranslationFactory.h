//~ RPR copyright

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "GLTFTypedefs.h"
#include "IGLTFMaterialTranslationFactory.generated.h"

UCLASS(hidecategories = Object)
class UGLTFMaterialTranslationFactoryInterface : public UFactory
{
    GENERATED_UCLASS_BODY()

    //~ UFactory interface

    virtual bool DoesSupportClass(UClass* InClass) override;

public:

    /** Initializes the factory with glTF material structures. */
    virtual bool InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial) PURE_VIRTUAL(UGLTFMaterialTranslationFactoryInterface::InitFromGLTF, return false;);

protected:

    /** Create and set up a scalar constant expression. */
    UMaterialExpressionConstant* CreateScalarExpression(const float& InScalar);

    /** Create and set up a Vector4 constant expression. */
    UMaterialExpressionConstant4Vector* CreateVec4Expression(const FLinearColor& InVector4);

    /** Create and set up a TextureSample expression. */
    UMaterialExpressionTextureSample* CreateTextureSampleExpression(UTexture2D* InTexture);

    /** Create and set up a multiply expression node. */
    UMaterialExpressionMultiply* CreateMultiplyExpression(UMaterialExpression* InA, UMaterialExpression* InB);

protected:

    /** The glTF master object. */
    FGLTFPtr GLTF;

    /** The glTF material. */
    GLTF::FMaterial GLTFMaterial;

    /** The UE4 material to translate to. */
    UMaterial* Material;

    /** The next Y-position to place a material expression node in the graph. */
    int32 MaterialNodeY;

    /** Increment MaterialNodeY. */
    int32 MaterialNodeStepY;
};
