#pragma once
#include "RPRToolsModule.h"
#include "RPRTypedefs.h"

namespace RPR
{
	namespace Context
	{
		RPRTOOLS_API FResult		Create(
			int32 ApiVersion,
			TArray<FPluginId>& PluginIds,
			FCreationFlags CreationFlags,
			const FContextProperties* ContextProperties,
			const FString& CachePath,
			FContext& OutContext
		);

		RPRTOOLS_API FResult		Create(
			int32 ApiVersion,
			FPluginId PluginId,
			FCreationFlags CreationFlags,
			const FContextProperties* ContextProperties,
			const FString& CachePath,
			FContext& OutContext
		);

		RPRTOOLS_API FResult		SetActivePlugin(FContext Context, FPluginId PluginId);


		namespace Parameters
		{
			RPRTOOLS_API FResult	Set1u(FContext Context, const FString& ParamName, uint32 Value);
			RPRTOOLS_API FResult	Set1f(FContext Context, const FString& ParamName, float Value);
		}

		namespace MaterialSystem
		{
			RPRTOOLS_API FResult	Create(RPR::FContext Context, RPR::FMaterialSystemType Type, RPR::FMaterialSystem& OutMaterialSystem);
		}
	}
}