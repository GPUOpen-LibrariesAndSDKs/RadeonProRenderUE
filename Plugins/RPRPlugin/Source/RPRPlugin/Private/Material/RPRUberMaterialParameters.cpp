#include "RPRUberMaterialParameters.h"

static bool CanUseOnlyIfMapIsSet(const FRPRUberMaterialParameterBase* parameter)
{
	const FRPRMaterialMap* materialMap = (const FRPRMaterialMap*)parameter;
	return (materialMap->Texture != nullptr);
}

FRPRUberMaterialParameters::FRPRUberMaterialParameters()
	: Diffuse_Color(		TEXT("diffuse.color"),		RPRX_UBER_MATERIAL_DIFFUSE_COLOR,		EPreviewSupport::Supported, 1.0f)
	, Diffuse_Weight(		TEXT("diffuse.weight"),		RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT,		EPreviewSupport::Supported, 1.0f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Diffuse_Roughness(	TEXT("diffuse.roughness"),	RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS,	EPreviewSupport::Supported, 0.0f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)

	, Reflection_Color(						TEXT("reflection.color"),				RPRX_UBER_MATERIAL_REFLECTION_COLOR,				EPreviewSupport::NotSupported, 1.0f)
	, Reflection_Weight(					TEXT("reflection.weight"),				RPRX_UBER_MATERIAL_REFLECTION_WEIGHT,				EPreviewSupport::NotSupported, 0.0f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Reflection_Roughness(					TEXT("reflection.roughness"),			RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS,			EPreviewSupport::NotSupported, 0.5f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Reflection_Anisotropy(				TEXT("reflection.anisotropy"),			RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY,			EPreviewSupport::NotSupported, 0.0f)
	, Reflection_AnisotropyRotation(		TEXT("reflection.anisotropyRotation"),	RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION,	EPreviewSupport::NotSupported, 0.0f)
	, Reflection_Metalness(					TEXT("reflection.metalness"),			RPRX_UBER_MATERIAL_REFLECTION_METALNESS,			EPreviewSupport::NotSupported, 1.0f)

	, Reflection_Mode(FRPRMaterialEnum::Create<ERPRReflectionMode>(TEXT("reflection.mode"), RPRX_UBER_MATERIAL_REFLECTION_MODE, EPreviewSupport::NotSupported, ERPRReflectionMode::PBR))

	, Reflection_Ior(						TEXT("reflection.ior"),					RPRX_UBER_MATERIAL_REFLECTION_IOR, EPreviewSupport::NotSupported, 1.5f)

	, Refraction_Color(						TEXT("refraction.color"),		RPRX_UBER_MATERIAL_REFRACTION_COLOR,			EPreviewSupport::NotSupported, 1.0f)
	, Refraction_Weight(					TEXT("refraction.weight"),		RPRX_UBER_MATERIAL_REFRACTION_WEIGHT,			EPreviewSupport::NotSupported, 0.0f,		ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Refraction_Roughness(					TEXT("refraction.roughness"),	RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS,		EPreviewSupport::NotSupported, 0.5f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Refraction_Ior(						TEXT("refraction.ior"),			RPRX_UBER_MATERIAL_REFRACTION_IOR,				EPreviewSupport::NotSupported, 1.5f)
	, Refraction_IsThinSurface(				TEXT("refraction.thinSurface"), RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE,		EPreviewSupport::NotSupported, false)

	, Refraction_Mode(FRPRMaterialEnum::Create<ERPRRefractionMode>(TEXT("refraction.iorMode"), RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE, EPreviewSupport::NotSupported, ERPRRefractionMode::Separate))

	, Coating_Color(					TEXT("coating.color"),		RPRX_UBER_MATERIAL_COATING_COLOR,		EPreviewSupport::NotSupported, 1.0f)
	, Coating_Weight(					TEXT("coating.weight"),		RPRX_UBER_MATERIAL_COATING_WEIGHT,		EPreviewSupport::NotSupported, 0.0f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Coating_Roughness(				TEXT("coating.roughness"),	RPRX_UBER_MATERIAL_COATING_ROUGHNESS,	EPreviewSupport::NotSupported, 0.5f,	ERPRMConstantOrMapC1InterpretationMode::AsFloat4)
	, Coating_Metalness(				TEXT("coating.metalness"),	RPRX_UBER_MATERIAL_COATING_METALNESS,	EPreviewSupport::NotSupported, 1.0f)
	, Coating_Ior(						TEXT("coating.ior"),		RPRX_UBER_MATERIAL_COATING_IOR,			EPreviewSupport::NotSupported, 1.5f)

	, Coating_Mode(FRPRMaterialEnum::Create<ERPRReflectionMode>(TEXT("coating.mode"), RPRX_UBER_MATERIAL_COATING_MODE, EPreviewSupport::NotSupported, ERPRReflectionMode::PBR))

	, Emission_Color(					TEXT("emission.color"),		RPRX_UBER_MATERIAL_EMISSION_COLOR,	EPreviewSupport::NotSupported, 1.0f)
	, Emission_Weight(					TEXT("emission.weight"),	RPRX_UBER_MATERIAL_EMISSION_WEIGHT, EPreviewSupport::NotSupported, 0.0f, ERPRMConstantOrMapC1InterpretationMode::AsFloat4)

	, Emission_Mode(FRPRMaterialEnum::Create<ERPREmissionMode>(TEXT("emission.mode"), RPRX_UBER_MATERIAL_EMISSION_MODE, EPreviewSupport::NotSupported, ERPREmissionMode::SingleSided))

	, Transparency(TEXT("transparency"), RPRX_UBER_MATERIAL_TRANSPARENCY, EPreviewSupport::NotSupported, 0.0f)

	, Normal(		TEXT("normal"),			RPRX_UBER_MATERIAL_NORMAL,			EPreviewSupport::NotSupported, FCanUseParameter::CreateStatic(CanUseOnlyIfMapIsSet))
	, Bump(			TEXT("bump"),			RPRX_UBER_MATERIAL_BUMP,			EPreviewSupport::NotSupported, FCanUseParameter::CreateStatic(CanUseOnlyIfMapIsSet))
	, Displacement(	TEXT("displacement"),	RPRX_UBER_MATERIAL_DISPLACEMENT,	EPreviewSupport::NotSupported, FCanUseParameter::CreateStatic(CanUseOnlyIfMapIsSet))

	, SSS_Absorption_Color(		TEXT("sss.absorptionColor"),		RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR,		EPreviewSupport::NotSupported, 0.0f)
	, SSS_Scatter_Color(		TEXT("sss.scatterColor"),			RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR,			EPreviewSupport::NotSupported, 0.0f)
	, SSS_Absorption_Distance(	TEXT("sss.absorptionDistance"),		RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE,		EPreviewSupport::NotSupported, 0.0f)
	, SSS_Scatter_Distance(		TEXT("sss.scatterDistance"),		RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE,		EPreviewSupport::NotSupported, 0.0f)
	, SSS_Scatter_Direction(	TEXT("sss.scatterDirection"),		RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION,		EPreviewSupport::NotSupported, 0.0f)
	, SSS_Weight(				TEXT("sss.weight"),					RPRX_UBER_MATERIAL_SSS_WEIGHT,					EPreviewSupport::NotSupported, 0.0f)
	, SSS_SubSurface_Color(		TEXT("sss.subsurfaceColor"),		RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR,		EPreviewSupport::NotSupported, 1.0f)
	, SSS_IsMultiScatter(		TEXT("sss.multiScatter"),			RPRX_UBER_MATERIAL_SSS_MULTISCATTER,			EPreviewSupport::NotSupported, true)
{}
