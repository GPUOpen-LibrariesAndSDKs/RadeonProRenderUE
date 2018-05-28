#pragma once
#include "UnrealTypeTraits.h"

UENUM(BlueprintType)
enum class ERPRReflectionMode : uint8
{
	PBR = RPRX_UBER_MATERIAL_REFLECTION_MODE_PBR,
	Metalness = RPRX_UBER_MATERIAL_REFLECTION_MODE_METALNESS,
};

UENUM(BlueprintType)
enum class ERPREmissionMode : uint8
{
	SingleSided = RPRX_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED,
	DoubleSided = RPRX_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED
};

UENUM(BlueprintType)
enum class ERPRRefractionMode : uint8
{
	Separate = RPRX_UBER_MATERIAL_REFRACTION_MODE_SEPARATE,
	Linked = RPRX_UBER_MATERIAL_REFRACTION_MODE_LINKED
};

//Expose_TNameOf(ERPRReflectionMode)
//Expose_TNameOf(ERPREmissionMode)
//Expose_TNameOf(ERPRRefractionMode)

template<>
struct TNameOf<ERPRReflectionMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPRReflectionMode");
	}
};

template<>
struct TNameOf<ERPREmissionMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPREmissionMode");
	}
};

template<>
struct TNameOf<ERPRRefractionMode>
{ 
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPRRefractionMode");
	}
};