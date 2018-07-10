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

#include "Factories/GLTFMaterialTranslationFactoryRPR.h"

#include "Factories/MaterialFactoryNew.h"
#include "MaterialGraph/MaterialGraph.h"
#include "MaterialGraph/MaterialGraphSchema.h"

#include "AssetRegistryModule.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "ComponentReregisterContext.h"
#include "PackageTools.h"
#include "Misc/Paths.h"

#include "GLTF.h"

#include "MaterialGraph/RPRMaterialGLTFGraph.h"
#include "Material/RPRUberMaterialParameters.h"

#include "RPRGLTFMaterialExpressionsHelper.h"

#include "RPRGLTFImporterModule.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialFactory.h"
#include "RPRMaterial.h"

#define LOCTEXT_NAMESPACE "UGLTFMaterialTranslationFactoryRPR"

UGLTFMaterialTranslationFactoryRPR::UGLTFMaterialTranslationFactoryRPR(const FObjectInitializer& InObjectInitializer)
    : Super(InObjectInitializer)
{
}

FText UGLTFMaterialTranslationFactoryRPR::GetDisplayName() const
{
    return LOCTEXT("GLTFMaterialTranslationFactoryRPR_Name", "Radeon ProRender glTF Material Translator");
}

UObject* UGLTFMaterialTranslationFactoryRPR::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, UObject* InContext, FFeedbackContext* InWarn)
{
    if (!GLTF.IsValid())
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactoryRPR::FactoryCreateNew: Not initialized with AMD_RPR_material structures."));
        return nullptr;
    }

	URPRMaterialFactory* rprMaterialFactory = NewObject<URPRMaterialFactory>();
	URPRMaterial* NewMaterial = Cast<URPRMaterial>(rprMaterialFactory->FactoryCreateNew(URPRMaterial::StaticClass(), InParent, InName, InFlags, nullptr, InWarn));

	if (!IsValid(NewMaterial))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactoryRPR::FactoryCreateNew: Error creating UMaterial."));
        return nullptr;
    }

	Material = NewMaterial;

	// Parse glTF structures into a graph hierarchy
    FRPRMaterialGLTFGraph MaterialGLTFGraph;
    if (!MaterialGLTFGraph.Parse(GLTFRPRMaterial))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactoryRPR::FactoryCreateNew: Could not parse the AMD_RPR_material structures."));
        return nullptr;
    }

    // The final structure containing Uber Material parameters
    FRPRUberMaterialParameters UberMaterialParams;
    // The serialization context that is passed through the graph structure to fill in the Uber Material parameters
    FRPRMaterialGraphSerializationContext SerializationContext;
    SerializationContext.ImportedFilePath = GLTF->Settings->FilePathInOS;
    SerializationContext.MaterialParameters = &NewMaterial->MaterialParameters;
    SerializationContext.MaterialGraph = &MaterialGLTFGraph;
    // Begin parsing the glTF graph into UberMaterialParameters
    MaterialGLTFGraph.Load(SerializationContext);

	URPRMaterialFactory::CopyRPRMaterialParameterToMaterialInstance(NewMaterial);

    FAssetRegistryModule::AssetCreated(NewMaterial);
    InParent->MarkPackageDirty();

    return NewMaterial;
}

bool UGLTFMaterialTranslationFactoryRPR::InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial)
{
    FGLTFPtr TheGLTF;
    if (!FRPRGLTFImporterModule::GetGLTF(TheGLTF))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactoryRPR::InitFromGLTF: glTF context is not valid."));
        return false;
    }
    if (!IsValid(TheGLTF->Settings))
    {
        UE_LOG(LogRPRRPRGLTFImporter, Error, TEXT("UGLTFMaterialTranslationFactoryRPR::InitFromGLTF: glTF import options are not set."));
        return false;
    }

    GLTF = TheGLTF;
    GLTFMaterial = InGLTFMaterial;

    // Return whether GLTF material contains an RPR material
    return amd::ImportExtension(GLTFMaterial.extensions, GLTFRPRMaterial);
}

void UGLTFMaterialTranslationFactoryRPR::TranslateRPRUberMaterialParameters(const FRPRUberMaterialParameters& InUberMatParams)
{
//
//	URPRMaterial* rprMaterial = Cast<URPRMaterial>(Material);
//
//	
//	rprMaterial->UpdateOverridableBaseProperties();
//
//	FMaterialInstanceBasePropertyOverrides& propertyOverrides = rprMaterial->BasePropertyOverrides;
//
//#define OVERRIDE_PROPERTY_VALUE(PropertyOverride, PropertyName, Value) \
//	PropertyOverride.bOverride_#PropertyName = true; \
//	PropertyOverride.#PropertyName = Value;
//
//    // Set whether to use double sided materials from the glTF structure
//	propertyOverrides.bOverride_TwoSided = true;
//	propertyOverrides.TwoSided = GLTF->Settings->TwoSidedMaterials && GLTFMaterial.doubleSided;
//
//    // First determine Shading Model based on presence of SSS or Coating layer
//    /** Uber SSS layer */
//    const auto& SSS_Weight = InUberMatParams.SSS_Weight;
//    bool bUseSSSLayer = SSS_Weight.Constant != 0.f;
//    if (bUseSSSLayer)
//    {
//        Material->SetShadingModel(MSM_Subsurface);
//        // Set Subsurface Color expression
//        const auto& SSS_Scatter_Color = InUberMatParams.SSS_Scatter_Color;
//        auto SubsurfaceColorExp = CreateMapExpression(SSS_Scatter_Color);
//        auto WeightExp = CreateScalarExpression(SSS_Weight.Constant);
//        auto MultiplyExp = CreateMultiplyExpression(SubsurfaceColorExp, WeightExp);
//        Material->SubsurfaceColor.Expression = MultiplyExp;
//        // Set Opacity expression based on Scatter Distance
//        const auto& SSS_Scatter_Distance = InUberMatParams.SSS_Scatter_Distance;
//        auto OpacityExp = CreateScalarExpression(SSS_Scatter_Distance.Constant);
//        Material->Opacity.Expression = OpacityExp;
//    }
//
//    /** Uber Coating layer (ignored if SSS layer exists) */
//    const auto& Coating_Weight = InUberMatParams.Coating_Weight;
//    bool bUseCoatingLayer = !bUseSSSLayer
//        && ((Coating_Weight.Mode == ERPRMaterialMapMode::Constant && Coating_Weight.Constant > 0.f)
//            || Coating_Weight.Mode == ERPRMaterialMapMode::Texture);
//    if (bUseCoatingLayer)
//    {
//        Material->SetShadingModel(MSM_ClearCoat);
//        // Set Clear Coat expression (based on Weight and not Color, as UE4 considers Coating to always be white)
//        auto ClearCoatExp = CreateMapExpression(Coating_Weight);
//        Material->ClearCoat.Expression = ClearCoatExp;
//        // Set Clear Coat Roughness expression
//        const auto& Coating_Roughness = InUberMatParams.Coating_Roughness;
//        auto ClearCoatRoughnessExp = CreateMapExpression(Coating_Roughness);
//        Material->ClearCoatRoughness.Expression = ClearCoatRoughnessExp;
//    }
//
//    /** Uber Transparency layer (ignored if SSS layer exists) */
//    if (Material->GetShadingModel() == MSM_DefaultLit || Material->GetShadingModel() == MSM_ClearCoat)
//    {
//        const auto& Transparency = InUberMatParams.Transparency;
//        if (Transparency.Constant != 0.f)
//        {
//            Material->BlendMode = BLEND_Translucent;
//            auto OpacityExp = CreateScalarExpression(Transparency.Constant);
//            Material->Opacity.Expression = OpacityExp;
//        }
//        else
//        {
//            Material->BlendMode = BLEND_Opaque;
//        }
//    }
//
//    /** Uber Diffuse layer & Reflection layer */
//    // Uber can have separate reflection and diffuse colors while UE4 has one, so do this
//    const auto& Diffuse_Color = InUberMatParams.Diffuse_Color;
//    const auto& Diffuse_Weight = InUberMatParams.Diffuse_Weight;
//    const auto& Diffuse_Roughness = InUberMatParams.Diffuse_Roughness;
//    const auto& Reflection_Color = InUberMatParams.Reflection_Color;
//    const auto& Reflection_Weight = InUberMatParams.Reflection_Weight;
//    const auto& Reflection_Roughness = InUberMatParams.Reflection_Roughness;
//    // If diffuse weight is 0, use reflection parameters for base
//    if (Diffuse_Weight.Mode == ERPRMaterialMapMode::Constant && Diffuse_Weight.Constant > 0.f)
//    {
//        auto BaseColorExp = CreateMapExpression(Reflection_Color);
//        Material->BaseColor.Expression = BaseColorExp;
//        auto MetallicExp = CreateScalarExpression(1.f);
//        Material->Metallic.Expression = MetallicExp;
//        auto RoughnessExp = CreateMapExpression(Reflection_Roughness);
//        Material->Roughness.Expression = RoughnessExp;
//        auto SpecularExp = CreateMapExpression(Reflection_Weight);
//        Material->Specular.Expression = SpecularExp;
//    }
//    // If reflection weight is 0, use diffuse parameters for base
//    else if (Reflection_Weight.Mode == ERPRMaterialMapMode::Constant && Reflection_Weight.Constant > 0.f)
//    {
//        auto BaseColorExp = CreateMapExpression(Diffuse_Color);
//        Material->BaseColor.Expression = BaseColorExp;
//        auto MetallicExp = CreateScalarExpression(0.f);
//        Material->Metallic.Expression = MetallicExp;
//        auto RoughnessExp = CreateMapExpression(Diffuse_Roughness);
//        Material->Roughness.Expression = RoughnessExp;
//        auto SpecularExp = CreateScalarExpression(0.f);
//        Material->Specular.Expression = SpecularExp;
//    }
//    else
//    {
//        const auto& Reflection_Mode = InUberMatParams.Reflection_Mode;
//        const auto& Reflection_Metalness = InUberMatParams.Reflection_Metalness;
//
//        auto BaseColorExp = CreateMapExpression(Diffuse_Color);
//        Material->BaseColor.Expression = BaseColorExp;
//        auto RoughnessExp = CreateMapExpression(Reflection_Roughness);
//        Material->Roughness.Expression = RoughnessExp;
//        auto SpecularExp = CreateMapExpression(Reflection_Weight);
//        Material->Specular.Expression = SpecularExp;
//
//        // Use metalness if reflection mode is metalness, otherwise 0
//        auto MetallicExp = Reflection_Mode.EnumValue == StaticCast<uint8>(ERPRReflectionMode::Metalness)
//            ? StaticCast<UMaterialExpression*>(CreateMapExpression(Reflection_Metalness))
//            : StaticCast<UMaterialExpression*>(CreateScalarExpression(0.f));
//        Material->Metallic.Expression = MetallicExp;
//    }
//
//    /** Uber Emission layer */
//    const auto& Emission_Weight = InUberMatParams.Emission_Weight;
//    bool bUseEmissionLayer =
//        (Emission_Weight.Mode == ERPRMaterialMapMode::Constant && Emission_Weight.Constant > 0.f)
//        || Emission_Weight.Mode == ERPRMaterialMapMode::Texture;
//    if (bUseEmissionLayer)
//    {
//        // Set EmissiveColor expression
//        const auto& Emission_Color = InUberMatParams.Emission_Color;
//        auto EmissiveColorExp = CreateMapExpression(Emission_Color);
//        auto WeightExp = CreateMapExpression(Emission_Weight);
//        auto MultiplyExp = CreateMultiplyExpression(EmissiveColorExp, WeightExp);
//        Material->EmissiveColor.Expression = MultiplyExp;
//    }
//
//    /** Uber Normal layer */
//    const auto& Normal = InUberMatParams.Normal;
//    bool bUseNormalLayer = Normal.Texture != nullptr;
//    if (bUseNormalLayer)
//    {
//        // Set Normal expression
//        auto NormalExp = CreateTextureSampleExpression(Normal.Texture);
//        NormalExp->SamplerType = EMaterialSamplerType::SAMPLERTYPE_LinearColor;
//        Material->Normal.Expression = NormalExp;
//    }
//
//    /** Uber Displacement layer */
//    const auto& Displacement = InUberMatParams.Displacement;
//    bool bUseDisplacementLayer = Displacement.Texture != nullptr;
//    if (bUseDisplacementLayer)
//    {
//        // WorldDisplacement requires Tessellation
//        Material->D3D11TessellationMode = EMaterialTessellationMode::MTM_FlatTessellation;
//        // Set WorldDisplacement expression
//        auto WorldDisplacementExp = CreateTextureSampleExpression(Displacement.Texture);
//        Material->WorldDisplacement.Expression = WorldDisplacementExp;
//    }

    // TODO: Find out if it is possible to do Bump Maps separately from Normal Maps
    // Can potentially have Bump Map as a TextureSample that inputs as Height into a BumpOffset node,
    // which then we would use that for the UVs of all the other texture samples used in the material graph
}

UMaterialExpression* UGLTFMaterialTranslationFactoryRPR::CreateMapExpression(const FRPRMaterialCoM& InMap)
{
    return InMap.Mode == ERPRMaterialMapMode::Constant
        ? StaticCast<UMaterialExpression*>(CreateVec4Expression(InMap.Constant))
        : StaticCast<UMaterialExpression*>(CreateTextureSampleExpression(InMap.Texture));
}

UMaterialExpression* UGLTFMaterialTranslationFactoryRPR::CreateMapExpression(const FRPRMaterialCoMChannel1& InMap)
{
	return InMap.Mode == ERPRMaterialMapMode::Constant
		? StaticCast<UMaterialExpression*>(CreateScalarExpression(InMap.Constant))
		: StaticCast<UMaterialExpression*>(CreateTextureSampleExpression(InMap.Texture));
}

#undef LOCTEXT_NAMESPACE
