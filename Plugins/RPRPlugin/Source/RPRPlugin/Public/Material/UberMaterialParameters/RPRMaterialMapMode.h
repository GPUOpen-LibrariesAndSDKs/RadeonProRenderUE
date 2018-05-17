#pragma once
#include "UnrealTypeTraits.h"
#include "ObjectMacros.h"

UENUM(BlueprintType)
enum class ERPRMaterialMapMode : uint8
{
	Constant,
	Texture
};

template<>
struct TNameOf<ERPRMaterialMapMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPRMaterialMapMode");
	}
};