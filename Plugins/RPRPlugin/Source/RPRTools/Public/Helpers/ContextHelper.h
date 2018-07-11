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
#pragma once
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"

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

		RPRTOOLS_API FResult		CreateScene(FContext Context, FScene& OutScene);
		RPRTOOLS_API FResult		SetActivePlugin(FContext Context, FPluginId PluginId);
		RPRTOOLS_API FResult		ClearMemory(FContext Context);


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
