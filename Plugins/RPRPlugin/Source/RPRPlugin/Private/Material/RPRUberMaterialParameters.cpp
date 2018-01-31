#include "RPRUberMaterialParameters.h"

FRPRMaterialMap::FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, float UniformConstant)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
{}

FRPRMaterialMapChannel1::FRPRMaterialMapChannel1(const FString& InXmlParamName, uint32 InRprxParamID, float InConstantValue)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID)
	, Constant(1.0f)
{}

FRPRMaterialNormal::FRPRMaterialNormal(const FString& InXmlParamName, uint32 InRprxParamID)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID)
	, bIsBump(false)
{}

FRPRUberMaterialParameters::FRPRUberMaterialParameters()
	: Diffuse_Color(		TEXT("diffuse.color"),		RPRX_UBER_MATERIAL_DIFFUSE_COLOR, 1.0f)
	, Diffuse_Weight(		TEXT("diffuse.weight"),		RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT, 1.0f)
	, Diffuse_Roughness(	TEXT("diffuse.roughness"),	RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS, 0.0f)

	, Reflection_Color(					TEXT("reflection.color"),				RPRX_UBER_MATERIAL_REFLECTION_COLOR, 1.0f)
	, Reflection_Weight(				TEXT("reflection.weight"),				RPRX_UBER_MATERIAL_REFLECTION_WEIGHT, 0.0f)
	, Reflection_Roughness(				TEXT("reflection.roughness"),			RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS, 0.5f)
	, Reflection_Anisotropy(			TEXT("reflection.anisotropy"),			RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY, 0.0f)
	, Reflection_AnisotropyRotation(	TEXT("reflection.anisotropyRotation"),	RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION, 0.0f)
	, Reflection_Metalness(				TEXT("reflection.metalness"),			RPRX_UBER_MATERIAL_REFLECTION_METALNESS, 1.0f)
	, Reflection_Mode(ERPRReflectionMode::PBR)
	, Reflection_Ior(					TEXT("reflection.ior"),					RPRX_UBER_MATERIAL_REFLECTION_IOR, 1.5f)

	, Refraction_Color(		TEXT("refraction.color"),		RPRX_UBER_MATERIAL_REFRACTION_COLOR, 1.0f)
	, Refraction_Weight(	TEXT("refraction.weight"),		RPRX_UBER_MATERIAL_REFRACTION_WEIGHT, 0.0f)
	, Refraction_Roughness(	TEXT("refraction.roughness"),	RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS, 0.5f)
	, Refraction_Ior(		TEXT("refraction.ior"),			RPRX_UBER_MATERIAL_REFRACTION_IOR, 1.5f)
	, Refraction_Mode(ERPRReflectionMode::PBR)
	, Refraction_IsThinSurface(false)

	, Coating_Color(		TEXT("coating.color"),		RPRX_UBER_MATERIAL_COATING_COLOR, 1.0f)
	, Coating_Weight(		TEXT("coating.weight"),		RPRX_UBER_MATERIAL_COATING_WEIGHT, 0.0f)
	, Coating_Roughness(	TEXT("coating.roughness"),	RPRX_UBER_MATERIAL_COATING_ROUGHNESS, 0.5f)
	, Coating_Metalness(	TEXT("coating.metalness"),	RPRX_UBER_MATERIAL_COATING_METALNESS, 1.0f)
	, Coating_Mode(ERPRReflectionMode::PBR)
	, Coating_Ior(			TEXT("coating.ior"),		RPRX_UBER_MATERIAL_COATING_IOR, 1.5f)

	, Emission_Color(		TEXT("emission.color"),		RPRX_UBER_MATERIAL_EMISSION_COLOR, 1.0f)
	, Emission_Weight(		TEXT("emission.weight"),	RPRX_UBER_MATERIAL_EMISSION_WEIGHT, 0.0f)
	, Emission_Mode(ERPREmissionMode::SingleSided)

	, Transparency(TEXT("transparency"), RPRX_UBER_MATERIAL_TRANSPARENCY, 0.0f)

	, SSS_Absorption_Color(		TEXT("sss.absorptionColor"),		RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR, 0.0f)
	, SSS_Scatter_Color(		TEXT("sss.scatterColor"),			RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR, 0.0f)
	, SSS_Absorption_Distance(	TEXT("sss.absorptionDistance"),		RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE, 0.0f)
	, SSS_Scatter_Distance(		TEXT("sss.scatterDistance"),		RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE, 0.0f)
	, SSS_Scatter_Direction(	TEXT("sss.scatterDirection"),		RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION, 0.0f)
	, SSS_Weight(				TEXT("sss.weight"),					RPRX_UBER_MATERIAL_SSS_WEIGHT, 0.0f)
	, SSS_SubSurface_Color(		TEXT("sss.subsurfaceColor"),		RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR, 1.0f)
	, SSS_IsMultiScatter(true)
{}

