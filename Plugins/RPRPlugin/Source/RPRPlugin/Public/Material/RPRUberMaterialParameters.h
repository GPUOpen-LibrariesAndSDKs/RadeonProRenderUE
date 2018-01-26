#pragma once

#include "Engine/Texture2D.h"
#include "RPRTypedefs.h"
#include "Color.h"
#include "RPRUberMaterialParameters.generated.h"


UENUM(BlueprintType)
enum class ERPRReflectionMode : uint8
{
	PBR,
	Metalness,
};

UENUM(BlueprintType)
enum class ERPREmissionMode : uint8
{
	SingleSided,
	DoubleSided
};

UENUM(BlueprintType)
enum class ERPRRefractionMode : uint8
{
	Separate,
	Linked
};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UTexture2D*		Texture;
};


USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMap : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor	Constant;	

	FRPRMaterialMap(float UniformConstant = 1.0f);

};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMapChannel1 : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	FRPRMaterialMapChannel1(float InConstantValue = 1.0f);

};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialNormal : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	bool		bIsBump;

	FRPRMaterialNormal();

};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameters
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "diffuse.color", rprxParam = RPRX_UBER_MATERIAL_DIFFUSE_COLOR))
	FRPRMaterialMap			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "diffuse.weight", rprxParam = RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT))
	FRPRMaterialMap			Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "diffuse.roughness", rprxParam = RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS))
	FRPRMaterialMap			Diffuse_Roughness;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.color", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_COLOR))
	FRPRMaterialMap			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.weight", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_WEIGHT))
	FRPRMaterialMap			Reflection_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.roughness", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS))
	FRPRMaterialMap			Reflection_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.anisotropy", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY))
	FRPRMaterialMapChannel1	Reflection_Anisotropy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.anisotropyRotation", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION))
	FRPRMaterialMapChannel1	Reflection_AnisotropyRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.metalness", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_METALNESS))
	FRPRMaterialMapChannel1	Reflection_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.mode", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_MODE))
	ERPRReflectionMode		Reflection_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.ior", rprxParam = RPRX_UBER_MATERIAL_REFLECTION_IOR))
	FRPRMaterialMapChannel1	Reflection_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.ior", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_COLOR))
	FRPRMaterialMap			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.weight", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_WEIGHT))
	FRPRMaterialMap			Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.roughness", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS))
	FRPRMaterialMap			Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.ior", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_IOR))
	FRPRMaterialMapChannel1	Refraction_Ior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.iorMode", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE))
	ERPRRefractionMode		Refraction_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.thinSurface", rprxParam = RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE))
	bool					Refraction_IsThinSurface;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.ior", rprxParam = RPRX_UBER_MATERIAL_COATING_COLOR))
	FRPRMaterialMap			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.weight", rprxParam = RPRX_UBER_MATERIAL_COATING_WEIGHT))
	FRPRMaterialMapChannel1	Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.roughness", rprxParam = RPRX_UBER_MATERIAL_COATING_ROUGHNESS))
	FRPRMaterialMapChannel1	Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.metalness", rprxParam = RPRX_UBER_MATERIAL_COATING_METALNESS))
	FRPRMaterialMapChannel1	Coating_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.mode", rprxParam = RPRX_UBER_MATERIAL_COATING_MODE))
	ERPRReflectionMode		Coating_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.ior", rprxParam = RPRX_UBER_MATERIAL_COATING_IOR))
	FRPRMaterialMapChannel1	Coating_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "emission.color", rprxParam = RPRX_UBER_MATERIAL_EMISSION_COLOR))
	FRPRMaterialMap			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "emission.weight", rprxParam = RPRX_UBER_MATERIAL_EMISSION_WEIGHT))
	FRPRMaterialMapChannel1	Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "emission.mode", rprxParam = RPRX_UBER_MATERIAL_EMISSION_MODE))
	ERPREmissionMode		Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "transparency", rprxParam = RPRX_UBER_MATERIAL_TRANSPARENCY))
	FRPRMaterialMapChannel1	Transparency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "normal", rprxParam = RPRX_UBER_MATERIAL_NORMAL))
	FRPRMaterialMap			Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "bump", rprxParam = RPRX_UBER_MATERIAL_BUMP))
	FRPRMaterialMap			Bump;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "displacement", rprxParam = RPRX_UBER_MATERIAL_BUMP))
	FRPRMaterialMap			Displacement;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.absorptionColor", rprxParam = RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR))
	FRPRMaterialMap			SSS_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.scatterColor", rprxParam = RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR))
	FRPRMaterialMap			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.absorptionDistance", rprxParam = RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE))
	FRPRMaterialMapChannel1	SSS_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.scatterDistance", rprxParam = RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE))
	FRPRMaterialMapChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.scatterDirection", rprxParam = RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION))
	FRPRMaterialMapChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.weight", rprxParam = RPRX_UBER_MATERIAL_SSS_WEIGHT))
	FRPRMaterialMapChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.subsurfaceColor", rprxParam = RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR))
	FRPRMaterialMap			SSS_SubSurface_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.multiScatter", rprxParam = RPRX_UBER_MATERIAL_SSS_MULTISCATTER))
	bool					SSS_IsMultiScatter;

	FRPRUberMaterialParameters();
};