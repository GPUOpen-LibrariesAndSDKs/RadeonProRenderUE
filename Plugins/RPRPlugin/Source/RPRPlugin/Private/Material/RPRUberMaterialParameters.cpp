#include "RPRUberMaterialParameters.h"

FRPRMaterialMap::FRPRMaterialMap(float UniformConstant)
	: Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
{}

FRPRMaterialMapChannel1::FRPRMaterialMapChannel1(float InConstantValue)
	: Constant(1.0f)
{}

FRPRMaterialNormal::FRPRMaterialNormal()
	: bIsBump(false)
{}

FRPRUberMaterialParameters::FRPRUberMaterialParameters()
	: Diffuse_Color(1.0f)
	, Diffuse_Weight(1.0f)
	, Diffuse_Roughness(0.0f)

	, Reflection_Color(1.0f)
	, Reflection_Weight(0.0f)
	, Reflection_Roughness(0.5f)
	, Reflection_Anisotropy(0.0f)
	, Reflection_AnisotropyRotation(0.0f)
	, Reflection_Metalness(1.0f)
	, Reflection_Mode(ERPRReflectionMode::PBR)
	, Reflection_Ior(1.5f)

	, Refraction_Color(1.0f)
	, Refraction_Weight(0.0f)
	, Refraction_Roughness(0.5f)
	, Refraction_Ior(1.5f)
	, Refraction_IsThinSurface(false)

	, Coating_Color(1.0f)
	, Coating_Weight(0.0f)
	, Coating_Roughness(0.5f)
	, Coating_Metalness(1.0f)
	, Coating_Mode(ERPRReflectionMode::PBR)
	, Coating_Ior(1.5f)

	, Emission_Color(1.0f)
	, Emission_Weight(0.0f)
	, Emission_Mode(ERPREmissionMode::SingleSided)

	, Transparency(0.0f)

	, SSS_Absorption_Color(0.0f)
	, SSS_Scatter_Color(0.0f)
	, SSS_Absorption_Distance(0.0f)
	, SSS_Scatter_Distance(0.0f)
	, SSS_Scatter_Direction(0.0f)
	, SSS_Weight(0.0f)
	, SSS_SubSurface_Color(1.0f)
	, SSS_IsMultiScatter(true)
{}

