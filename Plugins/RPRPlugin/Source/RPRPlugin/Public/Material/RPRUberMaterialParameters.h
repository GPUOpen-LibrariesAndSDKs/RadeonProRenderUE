#pragma once

#include "Engine/Texture2D.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName="diffuse.color"))
	FRPRMaterialMap			Diffuse_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "diffuse.normal"))
	FRPRMaterialNormal		Diffuse_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "diffuse.weight"))
	FRPRMaterialMap			Diffuse_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "diffuse.roughness"))
	FRPRMaterialMap			Diffuse_Roughness;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.color"))
	FRPRMaterialMap			Reflection_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.normal"))
	FRPRMaterialMap			Reflection_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.weight"))
	FRPRMaterialMap			Reflection_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.roughness"))
	FRPRMaterialMap			Reflection_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.anisotropy"))
	FRPRMaterialMapChannel1	Reflection_Anisotropy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.anisotropyRotation"))
	FRPRMaterialMapChannel1	Reflection_AnisotropyRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.metalness"))
	FRPRMaterialMapChannel1	Reflection_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.mode"))
	ERPRReflectionMode		Reflection_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "reflection.ior"))
	FRPRMaterialMapChannel1	Reflection_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.ior"))
	FRPRMaterialMap			Refraction_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.normal"))
	FRPRMaterialMap			Refraction_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.weight"))
	FRPRMaterialMap			Refraction_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.roughness"))
	FRPRMaterialMap			Refraction_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.ior"))
	FRPRMaterialMapChannel1	Refraction_Ior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.iorMode"))
	ERPRRefractionMode		Refraction_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "refraction.thinSurface"))
	bool					Refraction_IsThinSurface;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.ior"))
	FRPRMaterialMap			Coating_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.weight"))
	FRPRMaterialMapChannel1	Coating_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.roughness"))
	FRPRMaterialMapChannel1	Coating_Roughness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.metalness"))
	FRPRMaterialMapChannel1	Coating_Metalness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.mode"))
	ERPRReflectionMode		Coating_Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "coating.ior"))
	FRPRMaterialMapChannel1	Coating_Ior;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "emission.color"))
	FRPRMaterialMap			Emission_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "emission.weight"))
	FRPRMaterialMapChannel1	Emission_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "emission.mode"))
	ERPREmissionMode		Emission_Mode;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "transparency"))
	FRPRMaterialMapChannel1	Transparency;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.absorptionColor"))
	FRPRMaterialMap			SSS_Absorption_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.scatterColor"))
	FRPRMaterialMap			SSS_Scatter_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.absorptionDistance"))
	FRPRMaterialMapChannel1	SSS_Absorption_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.scatterDistance"))
	FRPRMaterialMapChannel1	SSS_Scatter_Distance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.scatterDirection"))
	FRPRMaterialMapChannel1	SSS_Scatter_Direction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.weight"))
	FRPRMaterialMapChannel1	SSS_Weight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.subsurfaceColor"))
	FRPRMaterialMap			SSS_SubSurface_Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material, meta = (XmlParamName = "sss.multiScatter"))
	bool					SSS_IsMultiScatter;

	FRPRUberMaterialParameters();

	void	Serialize(UMaterialInstance* MaterialInstance, bool bIsLoading);
};