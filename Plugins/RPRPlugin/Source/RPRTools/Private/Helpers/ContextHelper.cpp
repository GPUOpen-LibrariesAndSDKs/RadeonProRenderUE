#include "ContextHelper.h"

namespace RPR
{
	namespace Context
	{

		FResult Create(int32 ApiVersion, TArray<FPluginId>& PluginIds, FCreationFlags CreationFlags, const FContextProperties* ContextProperties, const FString& CachePath, FContext& OutContext)
		{
			return rprCreateContext(ApiVersion, PluginIds.GetData(), PluginIds.Num(), CreationFlags, ContextProperties, TCHAR_TO_ANSI(*CachePath), &OutContext);
		}

		FResult Create(int32 ApiVersion, FPluginId PluginId, FCreationFlags CreationFlags, const FContextProperties* ContextProperties, const FString& CachePath, FContext& OutContext)
		{
			TArray<FPluginId> pluginIds;
			pluginIds.Add(PluginId);
			return (Create(ApiVersion, pluginIds, CreationFlags, ContextProperties, CachePath, OutContext));
		}

		FResult SetActivePlugin(FContext Context, FPluginId PluginId)
		{
			return (rprContextSetActivePlugin(Context, PluginId));
		}


		namespace Parameters
		{
			FResult Set1u(FContext Context, const FString& ParamName, uint32 Value)
			{
				return (rprContextSetParameter1u(Context, TCHAR_TO_ANSI(*ParamName), Value));
			}

			FResult Set1f(FContext Context, const FString& ParamName, float Value)
			{
				return (rprContextSetParameter1f(Context, TCHAR_TO_ANSI(*ParamName), Value));
			}
		}

		namespace MaterialSystem
		{
			FResult MaterialSystem::Create(RPR::FContext Context, RPR::FMaterialSystemType Type, RPR::FMaterialSystem& OutMaterialSystem)
			{
				return (rprContextCreateMaterialSystem(Context, Type, &OutMaterialSystem));
			}
		}

	}
}