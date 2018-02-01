#pragma once

#include "Engine/Texture2D.h"
#include "RPRTypedefs.h"
#include "Color.h"
#include "UnrealTypeTraits.h"
#include "RPRMaterialParameterEnums.h"
#include "RPRUBerMaterialParameterBase.h"
#include "RPRUberMaterialParameters.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialBaseMap : public FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UTexture2D*		Texture;

	FRPRMaterialBaseMap() {}
	FRPRMaterialBaseMap(const FString& InXmlParamName, uint32 InRprxParamID);
};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMap : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor	Constant;	

	FRPRMaterialMap() {}
	FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, float UniformConstant = 1.0f);

};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMapChannel1 : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	FRPRMaterialMapChannel1() {}
	FRPRMaterialMapChannel1(const FString& InXmlParamName, uint32 InRprxParamID, float InConstantValue = 1.0f);

};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialBool : public FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	bool	bIsEnabled;

	FRPRMaterialBool() {}
	FRPRMaterialBool(const FString& InXmlParamName, uint32 InRprxParamID, bool DefaultValue);
};

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialEnum : public FRPRUberMaterialParameterBase
{
	friend struct FRPRUberMaterialParameters;

	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Material)
	uint8	EnumValue;

	UPROPERTY()
	UEnum*	EnumType;

	FRPRMaterialEnum() {}
	FRPRMaterialEnum(const FString& InXmlParamName, uint32 InRprxParamID);

	template<typename TEnumType>
	void	SetValue(TEnumType InEnumValue)
	{
		EnumValue = StaticCast<uint8>(InEnumValue);
	}

	void	SetRawValue(uint8 RawValue)
	{
		EnumValue = RawValue;
	}

private:

	template<typename TEnumType>
	void	Initialize(const FString& InXmlParamName, uint32 InRprxParamID, TEnumType InEnumValue)
	{
		SetValue<TEnumType>(InEnumValue);
		EnumType = FindObject<UEnum>((UObject*)ANY_PACKAGE, TNameOf<TEnumType>::GetName(), true);
		check(EnumType);
	}

};

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
	FRPRMaterialMapChannel1	Reflection_AnisotropyRotation;

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

private:
	template<typename EnumType>
	FRPRMaterialEnum	CreateEnum(const FString& InXmlParamName, uint32 InRprxParamID, EnumType InEnumValue)
	{
		FRPRMaterialEnum materialEnum(InXmlParamName, InRprxParamID);
		materialEnum.Initialize<EnumType>(InXmlParamName, InRprxParamID, InEnumValue);
		return (materialEnum);
	}
};

