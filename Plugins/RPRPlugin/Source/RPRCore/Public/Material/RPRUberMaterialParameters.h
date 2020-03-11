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

#pragma once

#include "Engine/Texture2D.h"
#include "Typedefs/RPRTypedefs.h"
#include "Math/Color.h"
#include "Templates/UnrealTypeTraits.h"
#include "Enums/RPRMaterialParameterEnums.h"
#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/UberMaterialParameters/RPRMaterialBool.h"
#include "Material/UberMaterialParameters/RPRMaterialEnum.h"
#include "Material/UberMaterialParameters/RPRMaterialNormalMap.h"
#include "RPRUberMaterialParameters.generated.h"

USTRUCT(BlueprintType)
struct RPRCORE_API FRPRUberMaterialParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialCoMChannel1	Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialCoM			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialNormalMap	Diffuse_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialCoMChannel1	Diffuse_Roughness;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoM			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialNormalMap	Reflection_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Anisotropy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_AnisotropyRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialEnum		Reflection_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Metalness;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Ior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoM			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoM			Refraction_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Ior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialBool		Refraction_Caustics;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialBool		Refraction_IsThinSurface;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoM			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoM			Coating_Transmission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialNormalMap	Coating_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Thickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Ior;

	// Only PBR supported
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialEnum		Coating_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialCoMChannel1	Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialCoM			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialEnum		Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Miscs")
	FRPRMaterialCoMChannel1	Transparency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Miscs")
	FRPRMaterialMap			Displacement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoM			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialBool		SSS_IsMultiScatter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Backscatter")
	FRPRMaterialCoMChannel1	Backscatter_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Backscatter")
	FRPRMaterialCoM			Backscatter_Color;



	FRPRUberMaterialParameters();

#if WITH_EDITOR
	void	SetupEditorSettings();
#endif

};

