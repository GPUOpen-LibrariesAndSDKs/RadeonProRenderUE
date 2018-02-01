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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse", meta = (XmlParamName = "diffuse.color", rprxParam = RPRX_UBER_MATERIAL_DIFFUSE_COLOR))
	FRPRMaterialMap			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse", meta = (XmlParamName = "diffuse.weight", rprxParam = RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT))
	FRPRMaterialMap			Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Diffuse", meta = (XmlParamName = "diffuse.roughness", rprxParam = RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS))
	FRPRMaterialMap			Diffuse_Roughness;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.color", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_COLOR))
	FRPRMaterialMap			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.weight", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_WEIGHT))
	FRPRMaterialMap			Reflection_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.roughness", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS))
	FRPRMaterialMap			Reflection_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.anisotropy", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY))
	FRPRMaterialMapChannel1	Reflection_Anisotropy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.anisotropyRotation", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION))
	FRPRMaterialMap			Reflection_AnisotropyRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.metalness", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_METALNESS))
	FRPRMaterialMap			Reflection_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.mode", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_MODE))
	FRPRMaterialEnum		Reflection_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Reflection", meta = (XmlParamName = "reflection.ior", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_IOR))
	FRPRMaterialMap			Reflection_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction", meta = (XmlParamName = "refraction.color", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_COLOR))
	FRPRMaterialMap			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction", meta = (XmlParamName = "refraction.weight", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_WEIGHT))
	FRPRMaterialMap			Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction", meta = (XmlParamName = "refraction.roughness", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS))
	FRPRMaterialMap			Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction", meta = (XmlParamName = "refraction.ior", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_IOR))
	FRPRMaterialMap			Refraction_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction", meta = (XmlParamName = "refraction.thinSurface", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE))
	FRPRMaterialBool		Refraction_IsThinSurface;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Refraction", meta = (XmlParamName = "refraction.iorMode", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE))
	FRPRMaterialEnum		Refraction_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating", meta = (XmlParamName = "coating.color", rprxParam = RPRX_UBER_MATERIAL_COATING_COLOR))
	FRPRMaterialMap			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating", meta = (XmlParamName = "coating.weight", rprxParam = RPRX_UBER_MATERIAL_COATING_WEIGHT))
	FRPRMaterialMap			Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating", meta = (XmlParamName = "coating.roughness", rprxParam = RPRX_UBER_MATERIAL_COATING_ROUGHNESS))
	FRPRMaterialMap			Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating", meta = (XmlParamName = "coating.metalness", rprxParam = RPRX_UBER_MATERIAL_COATING_METALNESS))
	FRPRMaterialMap			Coating_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating", meta = (XmlParamName = "coating.ior", rprxParam = RPRX_UBER_MATERIAL_COATING_IOR))
	FRPRMaterialMap			Coating_Ior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Coating", meta = (XmlParamName = "coating.mode", rprxParam = RPRX_UBER_MATERIAL_COATING_MODE))
	FRPRMaterialEnum		Coating_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission", meta = (XmlParamName = "emission.color", rprxParam = RPRX_UBER_MATERIAL_EMISSION_COLOR))
	FRPRMaterialMap			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission", meta = (XmlParamName = "emission.weight", rprxParam = RPRX_UBER_MATERIAL_EMISSION_WEIGHT))
	FRPRMaterialMap			Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Emission", meta = (XmlParamName = "emission.mode", rprxParam = RPRX_UBER_MATERIAL_EMISSION_MODE))
	FRPRMaterialEnum		Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Transparency", meta = (XmlParamName = "transparency", rprxParam = RPRX_UBER_MATERIAL_TRANSPARENCY))
	FRPRMaterialMapChannel1	Transparency;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal", meta = (XmlParamName = "normal", rprxParam = RPRX_UBER_MATERIAL_NORMAL))
	FRPRMaterialMap			Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal", meta = (XmlParamName = "bump", rprxParam = RPRX_UBER_MATERIAL_BUMP))
	FRPRMaterialMap			Bump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|Normal", meta = (XmlParamName = "displacement", rprxParam = RPRX_UBER_MATERIAL_DISPLACEMENT))
	FRPRMaterialMap			Displacement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.absorptionColor", rprxParam = RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR))
	FRPRMaterialMap			SSS_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.scatterColor", rprxParam = RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR))
	FRPRMaterialMap			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.absorptionDistance", rprxParam = RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE))
	FRPRMaterialMapChannel1	SSS_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.scatterDistance", rprxParam = RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE))
	FRPRMaterialMapChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.scatterDirection", rprxParam = RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION))
	FRPRMaterialMapChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.weight", rprxParam = RPRX_UBER_MATERIAL_SSS_WEIGHT))
	FRPRMaterialMapChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.subsurfaceColor", rprxParam = RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR))
	FRPRMaterialMap			SSS_SubSurface_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material|SSS", meta = (XmlParamName = "sss.multiScatter", rprxParam = RPRX_UBER_MATERIAL_SSS_MULTISCATTER))
	FRPRMaterialBool		SSS_IsMultiScatter;

	FRPRUberMaterialParameters();

};

