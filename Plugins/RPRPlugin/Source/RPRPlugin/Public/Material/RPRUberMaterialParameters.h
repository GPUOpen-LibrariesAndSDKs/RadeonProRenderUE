#pragma once

#include "Engine/Texture2D.h"
#include "RPRTypedefs.h"
#include "Color.h"
#include "UnrealTypeTraits.h"
#include "RPRMaterialParameterEnums.h"
#include "RPRUBerMaterialParameterBase.h"
#include "RPRMaterialMapChannel1.h"
#include "RPRMaterialMap.h"
#include "RPRMaterialBool.h"
#include "RPRMaterialEnum.h"
#include "RPRUberMaterialParameters.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialMap			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialMap			Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse")
	FRPRMaterialMap			Diffuse_Roughness;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMap			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMap			Reflection_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMap			Reflection_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMapChannel1	Reflection_Anisotropy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMap			Reflection_AnisotropyRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMap			Reflection_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialEnum		Reflection_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection")
	FRPRMaterialMap			Reflection_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialMap			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialMap			Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialMap			Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialMap			Refraction_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialBool		Refraction_IsThinSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction")
	FRPRMaterialEnum		Refraction_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialMap			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialMap			Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialMap			Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialMap			Coating_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialMap			Coating_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating")
	FRPRMaterialEnum		Coating_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialMap			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialMap			Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission")
	FRPRMaterialEnum		Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Transparency")
	FRPRMaterialMapChannel1	Transparency;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap			Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap			Bump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal")
	FRPRMaterialMap			Displacement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMap			SSS_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMap			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMapChannel1	SSS_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMapChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMapChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMapChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialMap			SSS_SubSurface_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS")
	FRPRMaterialBool		SSS_IsMultiScatter;



	FRPRUberMaterialParameters();

};

