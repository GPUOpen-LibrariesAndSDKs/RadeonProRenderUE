/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Material/RPRUberMaterialParameters.h"
#include "RadeonProRender.h"

#define LOCTEXT_NAMESPACE "RPRUberMaterialParameters"

static bool CanUseOnlyIfValidModeSet(const FRPRUberMaterialParameterBase* Parameter, FRPRMaterialEnum* EnumParameter, uint8 ExpectedValidMode)
{
	return (EnumParameter->EnumValue == ExpectedValidMode);
}

static bool CantUseParameter(const FRPRUberMaterialParameterBase* MaterialParameter)
{
	return (false);
}

FRPRUberMaterialParameters::FRPRUberMaterialParameters()
	// Diffuse
	: Diffuse_Weight(		TEXT("diffuse.weight"),		RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT,		ESupportMode::FullySupported, 1.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Diffuse_Color(		TEXT("diffuse.color"),		RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR,		ESupportMode::FullySupported, 1.0f)
	, Diffuse_Normal(		TEXT("diffuse.normal"),		RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL,		ESupportMode::FullySupported, FCanUseParameter())
	, Diffuse_Roughness(	TEXT("diffuse.roughness"),	RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS,	ESupportMode::NotSupported,	0.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)

	// Reflection
	, Reflection_Weight(				TEXT("reflection.weight"),				RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT,				ESupportMode::FullySupported,		1.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Reflection_Color(					TEXT("reflection.color"),				RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR,				ESupportMode::PreviewNotSupported, 1.0f)
	, Reflection_Normal(				TEXT("reflection.normal"),				RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL,				ESupportMode::PreviewNotSupported,  FCanUseParameter())
	, Reflection_Roughness(				TEXT("reflection.roughness"),			RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS,			ESupportMode::FullySupported,		0.25f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Reflection_Anisotropy(			TEXT("reflection.anisotropy"),			RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY,			ESupportMode::PreviewNotSupported,	0.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Reflection_AnisotropyRotation(	TEXT("reflection.anisotropyRotation"),	RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION,	ESupportMode::PreviewNotSupported,	0.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)

	, Reflection_Metalness(	TEXT("reflection.metalness"),	RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,	ESupportMode::FullySupported,		1.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4,	FCanUseParameter::CreateStatic(CanUseOnlyIfValidModeSet, &Reflection_Mode, (uint8) ERPRReflectionMode::Metalness))
	, Reflection_Ior(		TEXT("reflection.ior"),			RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR,			ESupportMode::PreviewNotSupported,	1.5f,	ERPRMCoMapC1InterpretationMode::AsFloat4,	FCanUseParameter::CreateStatic(CanUseOnlyIfValidModeSet, &Reflection_Mode, (uint8) ERPRReflectionMode::PBR))

	, Reflection_Mode(FRPRMaterialEnum::Create<ERPRReflectionMode>(TEXT("reflection.mode"), RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, ESupportMode::PreviewNotSupported, ERPRReflectionMode::PBR))

	// Refraction
	, Refraction_Color(						TEXT("refraction.color"),				RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR,				ESupportMode::PreviewNotSupported, 1.0f)
	, Refraction_Weight(					TEXT("refraction.weight"),				RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT,				ESupportMode::PreviewNotSupported, 0.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Refraction_Roughness(					TEXT("refraction.roughness"),			RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS,			ESupportMode::PreviewNotSupported, 0.5f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Refraction_Absorption_Color(			TEXT("refraction.absorptionColor"),		RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR,		ESupportMode::PreviewNotSupported, 0.0f)
	, Refraction_Absorption_Distance(		TEXT("refraction.absorptionDistance"),	RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE,	ESupportMode::PreviewNotSupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Refraction_Caustics(					TEXT("refraction.caustics"),			RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS,				ESupportMode::PreviewNotSupported, false)
	, Refraction_Ior(						TEXT("refraction.ior"),					RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR,					ESupportMode::PreviewNotSupported, 1.5f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Refraction_IsThinSurface(				TEXT("refraction.thinSurface"),			RPR_MATERIAL_INPUT_UBER_REFRACTION_THIN_SURFACE,			ESupportMode::PreviewNotSupported, false)

	// Coating
	, Coating_Color(						TEXT("coating.color"),				RPR_MATERIAL_INPUT_UBER_COATING_COLOR,					ESupportMode::PreviewNotSupported, 1.0f)
	, Coating_Weight(						TEXT("coating.weight"),				RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT,					ESupportMode::PreviewNotSupported, 0.0f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Coating_Normal(						TEXT("coating.normal"),				RPR_MATERIAL_INPUT_UBER_COATING_NORMAL,					ESupportMode::PreviewNotSupported, FCanUseParameter())
	, Coating_Transmission_Color(			TEXT("coating.transmissioncolor"),	RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR,		ESupportMode::PreviewNotSupported, 1.0f)
	, Coating_Thickness(					TEXT("coating.thickness"),			RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS,				ESupportMode::PreviewNotSupported, 1.0f)
	, Coating_Roughness(					TEXT("coating.roughness"),			RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS,				ESupportMode::PreviewNotSupported, 0.5f,	ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Coating_Ior(							TEXT("coating.ior"),				RPR_MATERIAL_INPUT_UBER_COATING_IOR,					ESupportMode::PreviewNotSupported, 1.5f,	ERPRMCoMapC1InterpretationMode::AsFloat4)

	, Coating_Mode(FRPRMaterialEnum::Create<ERPRCoatingMode>(TEXT("coating.mode"), RPR_MATERIAL_INPUT_UBER_COATING_MODE, ESupportMode::PreviewNotSupported, ERPRCoatingMode::PBR))

	// Emission
	, Emission_Color(	TEXT("emission.color"),		RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR,	ESupportMode::FullySupported, 1.0f)
	, Emission_Weight(	TEXT("emission.weight"),	RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, ESupportMode::FullySupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)

	, Emission_Mode(FRPRMaterialEnum::Create<ERPREmissionMode>(TEXT("emission.mode"), RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, ESupportMode::FullySupported, ERPREmissionMode::SingleSided))

	// Miscs
	, Transparency(	TEXT("transparency"),	RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, ESupportMode::PreviewNotSupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)

	// SSS
	, SSS_Weight(				TEXT("sss.weight"),					RPR_MATERIAL_INPUT_UBER_SSS_WEIGHT,					ESupportMode::PreviewNotSupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)
	, SSS_Scatter_Color(		TEXT("sss.scatterColor"),			RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_COLOR,			ESupportMode::PreviewNotSupported, 0.0f)
	, SSS_Scatter_Distance(		TEXT("sss.scatterDistance"),		RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DISTANCE,		ESupportMode::PreviewNotSupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)
	, SSS_Scatter_Direction(	TEXT("sss.scatterDirection"),		RPR_MATERIAL_INPUT_UBER_SSS_SCATTER_DIRECTION,		ESupportMode::PreviewNotSupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)
	, SSS_IsMultiScatter(		TEXT("sss.multiScatter"),			RPR_MATERIAL_INPUT_UBER_SSS_MULTISCATTER,			ESupportMode::PreviewNotSupported, true)

	// Backscatter
	, Backscatter_Weight(	TEXT("backscatter.weight"),	RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT,	ESupportMode::PreviewNotSupported, 0.0f, ERPRMCoMapC1InterpretationMode::AsFloat4)
	, Backscatter_Color(	TEXT("backscatter.color"),	RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR,	ESupportMode::PreviewNotSupported, 1.0f)

	// Sheen (Not implemented yet)
	//, Sheen(		TEXT("sheen"),			RPRX_UBER_MATERIAL_SHEEN, ESupportMode::PreviewNotSupported, //)
	//, Sheen_Tint(	TEXT("sheen.tint"),		RPRX_UBER_MATERIAL_SHEEN_TINT, ESupportMode::PreviewNotSupported, //)
	//, Sheen_Weight(	TEXT("sheen.weight"),	RPRX_UBER_MATERIAL_SHEEN_WEIGHT, ESupportMode::PreviewNotSupported, //)
{
#if WITH_EDITOR
	SetupEditorSettings();
#endif
}


#if WITH_EDITOR

void FRPRUberMaterialParameters::SetupEditorSettings()
{
	Diffuse_Weight.GetConstantRestriction().SetRange01();
	Reflection_Weight.GetConstantRestriction().SetRange01();
	Reflection_Metalness.GetConstantRestriction().SetRange01();
	Coating_Weight.GetConstantRestriction().SetRange01();
	Emission_Weight.GetConstantRestriction().SetRange01();
	Transparency.GetConstantRestriction().SetRange01();
	SSS_Weight.GetConstantRestriction().SetRange01();
	Backscatter_Weight.GetConstantRestriction().SetRange01();
	Refraction_Weight.GetConstantRestriction().SetRange01();

	Diffuse_Roughness.GetConstantRestriction().SetRange01();
	Reflection_Roughness.GetConstantRestriction().SetRange01();
	Refraction_Roughness.GetConstantRestriction().SetRange01();
	Coating_Roughness.GetConstantRestriction().SetRange01();

	Coating_Thickness.GetConstantRestriction().SetMinimum(0.0f);
	Coating_Thickness.GetConstantRestriction().SetMinimum(10.0f);

	Reflection_Anisotropy.GetConstantRestriction().SetMinimum(-1.0f);
	Reflection_Anisotropy.GetConstantRestriction().SetMaximum(1.0f);

	Reflection_AnisotropyRotation.GetConstantRestriction().SetMinimum(-1.0f);
	Reflection_AnisotropyRotation.GetConstantRestriction().SetMaximum(1.0f);

	Reflection_Ior.GetConstantRestriction().SetMinimum(0.0f);
	Refraction_Ior.GetConstantRestriction().SetMinimum(0.0f);
	Coating_Ior.GetConstantRestriction().SetMinimum(0.0f);

	SSS_Scatter_Distance.GetConstantRestriction().SetMinimum(0.0f);
	SSS_Scatter_Direction.GetConstantRestriction().SetRange(-1.0f, 1.0f);

}

#endif // WITH_EDITOR


#undef LOCTEXT_NAMESPACE
