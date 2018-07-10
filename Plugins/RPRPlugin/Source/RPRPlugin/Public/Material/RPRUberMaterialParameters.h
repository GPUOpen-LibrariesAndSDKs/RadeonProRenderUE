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
#pragma once

#include "Engine/Texture2D.h"
#include "Typedefs/RPRTypedefs.h"
#include "Math/Color.h"
#include "Templates/UnrealTypeTraits.h"
#include "Enums/RPRMaterialParameterEnums.h"
#include "Material/UberMaterialParameters/RPRUBerMaterialParameterBase.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/UberMaterialParameters/RPRMaterialBool.h"
#include "Material/UberMaterialParameters/RPRMaterialEnum.h"
#include "RPRUberMaterialParameters.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialCoM			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialCoMChannel1	Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialCoMChannel1	Diffuse_Roughness;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoM			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Weight;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialCoMChannel1	Reflection_Ior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoM			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialCoMChannel1	Refraction_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialBool		Refraction_IsThinSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialEnum		Refraction_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoM			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialCoMChannel1	Coating_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialEnum		Coating_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialCoM			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialCoMChannel1	Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialEnum		Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Transparency")
	FRPRMaterialCoMChannel1	Transparency;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap			Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap			Bump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap			Displacement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoM			SSS_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoM			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoMChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialCoM			SSS_SubSurface_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialBool		SSS_IsMultiScatter;



	FRPRUberMaterialParameters();

	void	SetupEditorSettings();

};

