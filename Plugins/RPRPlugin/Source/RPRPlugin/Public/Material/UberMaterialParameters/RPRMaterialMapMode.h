#pragma once
#include "UnrealTypeTraits.h"
#include "ObjectMacros.h"

UENUM(BlueprintType)
enum class ERPRMaterialMapMode : uint8
{
	Constant,
	Texture
};


#define GET_ENUM_NAME_CHECKED(EnumName) ((void)sizeof(EnumName), TEXT(#EnumName))

/*
* Make it possible to get the enum name using TNameOf<ERPRMaterialMapMode>::GetName()
*/
template<>
struct TNameOf<ERPRMaterialMapMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return GET_ENUM_NAME_CHECKED(ERPRMaterialMapMode);
	}
};

#undef GET_ENUM_NAME_CHECKED