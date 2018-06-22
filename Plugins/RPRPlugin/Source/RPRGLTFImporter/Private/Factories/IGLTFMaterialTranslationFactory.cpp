//~ RPR copyright

#include "Factories/IGLTFMaterialTranslationFactory.h"

#include "GLTF.h"

UGLTFMaterialTranslationFactoryInterface::UGLTFMaterialTranslationFactoryInterface(const FObjectInitializer& InObjectInitializer)
    : Super(InObjectInitializer)
    , GLTF(nullptr)
    , Material(nullptr)
    , MaterialNodeY(-150)
    , MaterialNodeStepY(180)
{
    SupportedClass = UMaterial::StaticClass();

    bCreateNew = false;
    bEditorImport = true;
}

bool UGLTFMaterialTranslationFactoryInterface::DoesSupportClass(UClass* InClass)
{
    return InClass == UMaterial::StaticClass();
}

UMaterialExpressionConstant* UGLTFMaterialTranslationFactoryInterface::CreateScalarExpression(const float& InScalar)
{
    auto ConstantExp = NewObject<UMaterialExpressionConstant>(Material);
    ConstantExp->R = InScalar;
    ConstantExp->MaterialExpressionEditorX = -400;
    ConstantExp->MaterialExpressionEditorY = MaterialNodeY;
    Material->Expressions.Add(ConstantExp);
    MaterialNodeY += MaterialNodeStepY;
    return ConstantExp;
}

UMaterialExpressionConstant4Vector* UGLTFMaterialTranslationFactoryInterface::CreateVec4Expression(const FLinearColor& InVector4)
{
    auto Vec4Exp = NewObject<UMaterialExpressionConstant4Vector>(Material);
    Vec4Exp->Constant = InVector4;
    Vec4Exp->MaterialExpressionEditorX = -400;
    Vec4Exp->MaterialExpressionEditorY = MaterialNodeY;
    Material->Expressions.Add(Vec4Exp);
    MaterialNodeY += MaterialNodeStepY;
    return Vec4Exp;
}

UMaterialExpressionTextureSample* UGLTFMaterialTranslationFactoryInterface::CreateTextureSampleExpression(UTexture2D* InTexture)
{
    auto TextureExp = NewObject<UMaterialExpressionTextureSample>(Material);
    TextureExp->Texture = InTexture;
    TextureExp->MaterialExpressionEditorX = -400;
    TextureExp->MaterialExpressionEditorY = MaterialNodeY;
    Material->Expressions.Add(TextureExp);
    MaterialNodeY += MaterialNodeStepY;
    return TextureExp;
}

UMaterialExpressionMultiply* UGLTFMaterialTranslationFactoryInterface::CreateMultiplyExpression(UMaterialExpression* InA, UMaterialExpression* InB)
{
    auto MultiplyExp = NewObject<UMaterialExpressionMultiply>(Material);
    MultiplyExp->A.Expression = InA;
    MultiplyExp->B.Expression = InB;
    MultiplyExp->MaterialExpressionEditorX = -400;
    MultiplyExp->MaterialExpressionEditorY = MaterialNodeY;
    Material->Expressions.Add(MultiplyExp);
    MaterialNodeY += MaterialNodeStepY;
    return MultiplyExp;
}
