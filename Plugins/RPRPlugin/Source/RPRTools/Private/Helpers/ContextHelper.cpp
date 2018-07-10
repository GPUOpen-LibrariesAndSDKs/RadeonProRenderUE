/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "Helpers/ContextHelper.h"

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
