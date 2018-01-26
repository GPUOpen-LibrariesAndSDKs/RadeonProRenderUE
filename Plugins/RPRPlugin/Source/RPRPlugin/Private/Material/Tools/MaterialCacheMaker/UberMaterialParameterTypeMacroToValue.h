#pragma once
#include "UnrealString.h"
#include "Map.h"



namespace RPR
{
	class FUberMaterialParameterTypeMacroToValue
	{
	public:

		static uint32	FindParameterTypeValueByName(const FString& ParameterName);

	private:
		static TMap<FString, uint32>&		Map();
	};
}
