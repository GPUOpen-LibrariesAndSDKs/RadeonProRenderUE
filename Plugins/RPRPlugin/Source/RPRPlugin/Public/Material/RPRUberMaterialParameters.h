#pragma once

#include "Engine/Texture2D.h"
#include "RPRTypedefs.h"
#include "Color.h"
#include "UnrealTypeTraits.h"
#include "RPRMaterialParameterEnums.h"
#include "RPRUBerMaterialParameterBase.h"
#include "RPRMaterialConstantOrMapChannel1.h"
#include "RPRMaterialMap.h"
#include "RPRMaterialConstantOrMap.h"
#include "RPRMaterialBool.h"
#include "RPRMaterialEnum.h"
#include "RPRUberMaterialParameters.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialConstantOrMap			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialConstantOrMapChannel1	Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialConstantOrMapChannel1	Diffuse_Roughness;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMap			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMapChannel1	Reflection_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMapChannel1	Reflection_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMapChannel1	Reflection_Anisotropy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMap			Reflection_AnisotropyRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMap			Reflection_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialEnum					Reflection_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialConstantOrMap			Reflection_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialConstantOrMap			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialConstantOrMapChannel1			Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialConstantOrMapChannel1			Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialConstantOrMap			Refraction_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialBool					Refraction_IsThinSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialEnum					Refraction_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialConstantOrMap			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialConstantOrMapChannel1	Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialConstantOrMapChannel1	Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialConstantOrMap			Coating_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialConstantOrMap			Coating_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialEnum					Coating_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialConstantOrMap			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialConstantOrMapChannel1	Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialEnum					Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Transparency")
	FRPRMaterialConstantOrMapChannel1	Transparency;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap						Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap						Bump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap						Displacement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMap			SSS_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMap			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMapChannel1	SSS_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMapChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMapChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMapChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialConstantOrMap			SSS_SubSurface_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialBool					SSS_IsMultiScatter;



	FRPRUberMaterialParameters();

};

