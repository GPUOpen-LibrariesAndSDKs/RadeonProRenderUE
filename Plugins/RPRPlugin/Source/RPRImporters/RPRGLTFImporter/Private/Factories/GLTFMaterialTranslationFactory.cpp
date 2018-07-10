/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#include "Factories/GLTFMaterialTranslationFactory.h"

#include "Factories/MaterialFactoryNew.h"
#include "Factories/TextureFactory.h"
#include "MaterialGraph/MaterialGraph.h"
#include "MaterialGraph/MaterialGraphSchema.h"

#include "AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ComponentReregisterContext.h"
#include "PackageTools.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/Base64.h"
#include "Misc/Paths.h"

#include "GLTF.h"

#include "RPRGLTFMaterialExpressionsHelper.h"

#include "RPRGLTFImporterModule.h"

#define LOCTEXT_NAMESPACE "UGLTFMaterialTranslationFactory"

UGLTFMaterialTranslationFactory::UGLTFMaterialTranslationFactory(const FObjectInitializer& InObjectInitializer)
    : Super(InObjectInitializer)
{
}

FText UGLTFMaterialTranslationFactory::GetDisplayName() const
{
    return LOCTEXT("GLTFMaterialFactory_Name", "glTF Material Translator");
}

UObject* UGLTFMaterialTranslationFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, UObject* InContext, FFeedbackContext* InWarn)
{
    if (!GLTF.IsValid())
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::FactoryCreateNew: Not initialized with glTF structures."));
        return nullptr;
    }

    auto MaterialFactory = NewObject<UMaterialFactoryNew>();
    UMaterial* NewMaterial = (UMaterial*)MaterialFactory->FactoryCreateNew(UMaterial::StaticClass(), InParent, InName, InFlags, nullptr, InWarn);
    if (!IsValid(NewMaterial))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::FactoryCreateNew: Error creating UMaterial."));
        return nullptr;
    }

    Material = NewMaterial;
    TranslateMaterial();

    LayoutMaterial(Material);
    Material->PostEditChange();
    FGlobalComponentReregisterContext RecreateComponents;

    if (!NewMaterial->MaterialGraph)
    {
		NewMaterial->MaterialGraph = CastChecked<UMaterialGraph>(FBlueprintEditorUtils::CreateNewGraph(Material, NAME_None, UMaterialGraph::StaticClass(), UMaterialGraphSchema::StaticClass()));
		NewMaterial->MaterialGraph->Material = NewMaterial;
    }

    FAssetRegistryModule::AssetCreated(Material);
    InParent->MarkPackageDirty();

    return Material;
}

bool UGLTFMaterialTranslationFactory::InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial)
{
    FGLTFPtr TheGLTF;
    if (!FRPRGLTFImporterModule::GetGLTF(TheGLTF))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::InitFromGLTF: glTF context is not valid."));
        return false;
    }
    if (!IsValid(TheGLTF->Settings))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::InitFromGLTF: glTF import options are not set."));
        return false;
    }

    GLTF = TheGLTF;
    GLTFMaterial = InGLTFMaterial;
    return true;
}

void UGLTFMaterialTranslationFactory::TranslateMaterial()
{
	UMaterial* material = Cast<UMaterial>(Material);

	material->TwoSided = GLTF->Settings->TwoSidedMaterials && GLTFMaterial.doubleSided;
    material->SetShadingModel(MSM_DefaultLit);

    // TODO: Use the inherited MaterialExpression helpers
    { /** BaseColor */
        const auto& GLTFPbrMetallicRoughness = GLTFMaterial.pbrMetallicRoughness;
        UMaterialExpression* MatExp = nullptr;
        UTexture* BaseColorTexture = nullptr;
        int index = GLTFPbrMetallicRoughness.baseColorTexture.index;
        // Use a texture source for the base color of the material
        if (index != -1)
        {
            BaseColorTexture = CreateTexture(index);
            if (BaseColorTexture)
            {
                auto TexSam = NewObject<UMaterialExpressionTextureSample>(Material);
                TexSam->Texture = BaseColorTexture;
                TexSam->MaterialExpressionEditorX = -400;
                TexSam->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(TexSam);
                MaterialNodeY += MaterialNodeStepY;

                FVector4 BaseColorFactor = FVector4(GLTFPbrMetallicRoughness.baseColorFactor[0], GLTFPbrMetallicRoughness.baseColorFactor[1], GLTFPbrMetallicRoughness.baseColorFactor[2], GLTFPbrMetallicRoughness.baseColorFactor[3]);
                // Multiply TextureSample by BaseColorFactor if it is not solid white (default)
                if (!(BaseColorFactor.X == 1.0f && BaseColorFactor.Y == 1.0f && BaseColorFactor.Z == 1.0f && BaseColorFactor.W == 1.0f))
                {
                    auto Vec4 = NewObject<UMaterialExpressionConstant4Vector>(Material);
                    Vec4->Constant = FLinearColor(BaseColorFactor);
                    Vec4->MaterialExpressionEditorX = -400;
                    Vec4->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Vec4);
                    MaterialNodeY += MaterialNodeStepY;

                    auto Mul = NewObject<UMaterialExpressionMultiply>(Material);
                    Mul->A.Expression = TexSam;
                    Mul->B.Expression = Vec4;
                    Mul->MaterialExpressionEditorX = -400;
                    Mul->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Mul);
                    MaterialNodeY += MaterialNodeStepY;
                    MatExp = Mul;
                }
                else
                    MatExp = TexSam;
            }
        }
        // If there is no texture, use the base color factor as a solid color for the material
        if (!BaseColorTexture)
        {
            auto Vec3 = NewObject<UMaterialExpressionConstant4Vector>(Material);
            Vec3->Constant = FLinearColor(FVector(GLTFPbrMetallicRoughness.baseColorFactor[0], GLTFPbrMetallicRoughness.baseColorFactor[1], GLTFPbrMetallicRoughness.baseColorFactor[2]));
            Vec3->MaterialExpressionEditorX = -400;
            Vec3->MaterialExpressionEditorY = MaterialNodeY;
            material->Expressions.Add(Vec3);
            MaterialNodeY += MaterialNodeStepY;
            MatExp = Vec3;
        }
        if (MatExp)
            material->BaseColor.Expression = MatExp;
    }
    { /** MetallicRoughness */
        const auto& GLTFPbrMetallicRoughness = GLTFMaterial.pbrMetallicRoughness;
        UMaterialExpression* MatExpMetal = nullptr;
        UMaterialExpression* MatExpRough = nullptr;
        UMaterialExpressionTextureSample* TexSam = nullptr;
        int index = GLTFPbrMetallicRoughness.metallicRoughnessTexture.index;
        // Use a texture source where the Blue channel is Metallic factor and Green channel is Roughness factor
        if (index != -1)
        {
            UTexture* MetallicRoughnessTexture = CreateTexture(index);
            if (MetallicRoughnessTexture)
            {
                TexSam = NewObject<UMaterialExpressionTextureSample>(Material);
                TexSam->Texture = MetallicRoughnessTexture;
                TexSam->MaterialExpressionEditorX = -400;
                TexSam->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(TexSam);
                MaterialNodeY += MaterialNodeStepY;
            }
        }
        // Get the scalar metallic & roughness factors
        float MetalFactor = GLTFPbrMetallicRoughness.metallicFactor;
        if (MetalFactor > 0.0f)
        {
            auto Metal = NewObject<UMaterialExpressionConstant>(Material);
            Metal->R = MetalFactor;
            Metal->MaterialExpressionEditorX = -400;
            Metal->MaterialExpressionEditorY = MaterialNodeY;
            material->Expressions.Add(Metal);
            MaterialNodeY += MaterialNodeStepY;

            // If there is a texture sample, multiply it by this
            if (TexSam)
            {
                auto Mul = NewObject<UMaterialExpressionMultiply>(Material);
                Mul->A.Expression = TexSam;
                Mul->A.OutputIndex = 3; // Blue channel
                Mul->B.Expression = Metal;
                Mul->MaterialExpressionEditorX = -400;
                Mul->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(Mul);
                MaterialNodeY += MaterialNodeStepY;
                MatExpMetal = Mul;
            }
            else
                MatExpMetal = Metal;
        }
        else if (TexSam)
        {
            MatExpMetal = TexSam;
            material->Metallic.OutputIndex = 3; // Blue channel
        }
        float RoughFactor = GLTFPbrMetallicRoughness.roughnessFactor;
        if (RoughFactor > 0.0f)
        {
            auto Rough = NewObject<UMaterialExpressionConstant>(Material);
            Rough->R = RoughFactor;
            Rough->MaterialExpressionEditorX = -400;
            Rough->MaterialExpressionEditorY = MaterialNodeY;
            material->Expressions.Add(Rough);
            MaterialNodeY += MaterialNodeStepY;

            // If there is a texture sample, multiply it by this
            if (TexSam)
            {
                auto Mul = NewObject<UMaterialExpressionMultiply>(Material);
                Mul->A.Expression = TexSam;
                Mul->A.OutputIndex = 2; // Green channel
                Mul->B.Expression = Rough;
                Mul->MaterialExpressionEditorX = -400;
                Mul->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(Mul);
                MaterialNodeY += MaterialNodeStepY;
                MatExpRough = Mul;
            }
            else
                MatExpRough = Rough;
        }
        else if (TexSam)
        {
            MatExpRough = TexSam;
            material->Roughness.OutputIndex = 2; // Green channel
        }
        if (MatExpMetal)
            material->Metallic.Expression = MatExpMetal;
        if (MatExpRough)
            material->Roughness.Expression = MatExpRough;
    }
    { /** Normal Map */
        const auto& GLTFNormalTexInfo = GLTFMaterial.normalTexture;
        int index = GLTFNormalTexInfo.index;
        if (index != -1)
        {
            UMaterialExpression* MatExp = nullptr;
            UTexture* NormalTexture = CreateTexture(index);
            if (NormalTexture)
            {
                auto TexSam = NewObject<UMaterialExpressionTextureSample>(Material);
                TexSam->Texture = NormalTexture;
                TexSam->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
                TexSam->MaterialExpressionEditorX = -400;
                TexSam->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(TexSam);
                MaterialNodeY += MaterialNodeStepY;

                float NormalScale = GLTFNormalTexInfo.scale;
                // Multiply normal vectors by normal scale factor
                if (NormalScale >= 0.0f && NormalScale != 1.0f) // Default is 1.0
                {
                    auto Scale = NewObject<UMaterialExpressionConstant>(Material);
                    Scale->R = NormalScale;
                    Scale->MaterialExpressionEditorX = -400;
                    Scale->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Scale);
                    MaterialNodeY += MaterialNodeStepY;

                    auto Mul = NewObject<UMaterialExpressionMultiply>(Material);
                    Mul->A.Expression = TexSam;
                    Mul->B.Expression = Scale;
                    Mul->MaterialExpressionEditorX = -400;
                    Mul->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Mul);
                    MaterialNodeY += MaterialNodeStepY;
                    MatExp = Mul;
                }
                else
                    MatExp = TexSam;
                if (MatExp)
                    material->Normal.Expression = MatExp;
            }
        }
    }
    { /** Occlusion Map */
        const auto& GLTFOcclusionTexInfo = GLTFMaterial.occlusionTexture;
        int index = GLTFOcclusionTexInfo.index;
        if (index != -1)
        {
            UMaterialExpression* MatExp = nullptr;
            UTexture* OcclusionTexture = CreateTexture(index);
            if (OcclusionTexture)
            {
                auto TexSam = NewObject<UMaterialExpressionTextureSample>(Material);
                TexSam->Texture = OcclusionTexture;
                TexSam->SamplerType = EMaterialSamplerType::SAMPLERTYPE_Grayscale;
                TexSam->MaterialExpressionEditorX = -400;
                TexSam->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(TexSam);
                MaterialNodeY += MaterialNodeStepY;

                float OcclusionStrength = GLTFOcclusionTexInfo.strength;
                bool NeedsMultiply = OcclusionStrength >= 0.0f && OcclusionStrength != 1.0f; // Default is 1.0
                                                                                             // Multiply occlusion red channel by occlusion strength
                if (NeedsMultiply)
                {
                    auto Strength = NewObject<UMaterialExpressionConstant>(Material);
                    Strength->R = OcclusionStrength;
                    Strength->MaterialExpressionEditorX = -400;
                    Strength->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Strength);
                    MaterialNodeY += MaterialNodeStepY;

                    auto Mul = NewObject<UMaterialExpressionMultiply>(Material);
                    Mul->A.Expression = TexSam;
                    Mul->A.OutputIndex = 1; // Red channel
                    Mul->B.Expression = Strength;
                    Mul->MaterialExpressionEditorX = -400;
                    Mul->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Mul);
                    MaterialNodeY += MaterialNodeStepY;
                    MatExp = Mul;
                }
                else
                    MatExp = TexSam;
                if (MatExp)
                {
                    material->AmbientOcclusion.Expression = MatExp;
                    if (!NeedsMultiply) // Hook up Texture Sample red pin if no multiply node is used
                        material->AmbientOcclusion.OutputIndex = 1; // Red channel
                }
            }
        }
    }
    { /** Emissive Map */
        const auto& GLTFEmissiveTexInfo = GLTFMaterial.emissiveTexture;
        int index = GLTFEmissiveTexInfo.index;
        if (index != -1)
        {
            UMaterialExpression* MatExp = nullptr;
            UTexture* EmissiveTexture = CreateTexture(index);
            if (EmissiveTexture)
            {
                auto TexSam = NewObject<UMaterialExpressionTextureSample>(Material);
                TexSam->Texture = EmissiveTexture;
                TexSam->MaterialExpressionEditorX = -400;
                TexSam->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(TexSam);
                MaterialNodeY += MaterialNodeStepY;

                FVector EmissiveColorFactor = FVector(GLTFMaterial.emissiveFactor[0], GLTFMaterial.emissiveFactor[1], GLTFMaterial.emissiveFactor[2]);
                // Multiply TextureSample by EmissiveColorFactor if it is not solid black (default, unemissive)
                if (!(EmissiveColorFactor.X == 0.0f && EmissiveColorFactor.Y == 0.0f && EmissiveColorFactor.Z == 0.0f))
                {
                    auto Vec3 = NewObject<UMaterialExpressionConstant4Vector>(Material);
                    Vec3->Constant = FLinearColor(EmissiveColorFactor);
                    Vec3->MaterialExpressionEditorX = -400;
                    Vec3->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Vec3);
                    MaterialNodeY += MaterialNodeStepY;

                    auto Mul = NewObject<UMaterialExpressionMultiply>(Material);
                    Mul->A.Expression = TexSam;
                    Mul->B.Expression = Vec3;
                    Mul->MaterialExpressionEditorX = -400;
                    Mul->MaterialExpressionEditorY = MaterialNodeY;
                    material->Expressions.Add(Mul);
                    MaterialNodeY += MaterialNodeStepY;
                    MatExp = Mul;
                }
                else
                    MatExp = TexSam;
                if (MatExp)
                    material->EmissiveColor.Expression = MatExp;
            }
        }
    }
    { /** Alpha */
        GLTF::FMaterial::AlphaMode AlphaMode = GLTFMaterial.alphaMode;
        switch (AlphaMode)
        {
        case GLTF::FMaterial::AlphaMode::OPAQUE:
            material->BlendMode = BLEND_Opaque;
            break;
        case GLTF::FMaterial::AlphaMode::MASK:
            material->BlendMode = BLEND_Masked;
            {
                // In masked mode, use AlphaCutoff as Opacity Mask. If the alpha value of the material is
                // greater than or equal to this, it is fully opaque, otherwise it is fully transparent
                auto AlphaCutoff = NewObject<UMaterialExpressionConstant>(Material);
                AlphaCutoff->R = GLTFMaterial.alphaCutoff;
                AlphaCutoff->MaterialExpressionEditorX = -400;
                AlphaCutoff->MaterialExpressionEditorY = MaterialNodeY;
                material->Expressions.Add(AlphaCutoff);
                material->OpacityMask.Expression = AlphaCutoff;
                MaterialNodeY += MaterialNodeStepY;
            }
            break;
        case GLTF::FMaterial::AlphaMode::BLEND:
            material->BlendMode = BLEND_Translucent;
            break;
        }
    }
}

UTexture* UGLTFMaterialTranslationFactory::CreateTexture(int InTextureIndex)
{
    if (!(InTextureIndex >= 0 && InTextureIndex < GLTF->Data->textures.size()))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::CreateTexture: glTF texture index out of bounds."));
        return nullptr;
    }

    const GLTF::FTexture& GLTFTexture = GLTF->Data->textures.at(InTextureIndex);
    int ImageIndex = GLTFTexture.source;

    if (!(ImageIndex >= 0 && ImageIndex < GLTF->Data->images.size()))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::CreateTexture: glTF image index out of bounds."));
        return nullptr;
    }

    const GLTF::FImage& GLTFImage = GLTF->Data->images.at(ImageIndex);

    UTexture* Texture = nullptr;
    // TODO: Use AssetToolsModule to import from path instead
    if (Texture == nullptr)
    {
        FString TextureUri = GLTFImage.uri.c_str();
        FString PackageName = MakeUniqueObjectName(nullptr, UPackage::StaticClass(), *(FPackageName::GetLongPackagePath(GLTF->Settings->FilePathInEngine) / FPaths::GetBaseFilename(TextureUri))).ToString();
        UPackage* Package = LoadPackage(nullptr, *PackageName, 0);
        if (!Package)
        {
            Package = CreatePackage(nullptr, *PackageName);
            Package->FullyLoad();
            Package->Modify();
        }

        bool bOutCanceled = false;
        FName TextureName = FPackageName::GetShortFName(*PackageName);
        UTextureFactory* TextureFactory = NewObject<UTextureFactory>();
        // Texture is embedded
        if (TextureUri.Contains("data:image/") && TextureUri.Contains(";base64,"))
        {
            const FString& TextureBase64Str = TextureUri.RightChop(TextureUri.Find(";base64,") + FString(";base64,").Len());
            TArray<uint8> TextureData;
            if (!FBase64::Decode(TextureBase64Str, TextureData))
            {
                UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactory::CreateTexture: Could not decode embedded texture %d"), GLTFTexture.source);
                return nullptr;
            }
            FString Type = TextureUri.Mid(TextureUri.Find("data:image/") + FString("data:image/").Len(), 3);
            if (Type.Equals("png", ESearchCase::IgnoreCase) || Type.Equals("jpg", ESearchCase::IgnoreCase))
            {
                const uint8* Ptr = TextureData.GetData();
                Texture = (UTexture2D*)TextureFactory->FactoryCreateBinary(UTexture2D::StaticClass(), Package, TextureName, RF_Public | RF_Standalone, nullptr, *Type, Ptr, Ptr + TextureData.Num(), GWarn);
            }
        }
        // Texture is in glTF buffer
        else if (GLTFImage.bufferView != -1)
        {
            const GLTF::FBufferView& GLTFBufferView = GLTF->Data->bufferViews[GLTFImage.bufferView];
            TArray<uint8> TextureData;
            GLTF->Buffers->Get(GLTFBufferView.buffer, GLTFBufferView.byteOffset, GLTFBufferView.byteLength, TextureData);
            FString Type = GLTFImage.mimeType == GLTF::FImage::MimeType::IMAGE_JPEG ? "jpg" : "png";
            const uint8* Ptr = TextureData.GetData();
            Texture = (UTexture2D*)TextureFactory->FactoryCreateBinary(UTexture2D::StaticClass(), Package, TextureName, RF_Public | RF_Standalone, nullptr, *Type, Ptr, Ptr + TextureData.Num(), GWarn);
        }
        // Texture is an image file
        else if (FPaths::FileExists(FPaths::GetPath(GLTF->Settings->FilePathInOS) / TextureUri))
        {
            Texture = (UTexture2D*)TextureFactory->FactoryCreateFile(UTexture2D::StaticClass(), Package, TextureName, RF_Public | RF_Standalone, FPaths::GetPath(GLTF->Settings->FilePathInOS) / TextureUri, nullptr, GWarn, bOutCanceled);
        }
        if (Texture && !bOutCanceled)
        {
            FAssetRegistryModule::AssetCreated(Texture);
            Package->MarkPackageDirty();
        }
    }
    return Texture;
}

#undef LOCTEXT_NAMESPACE
