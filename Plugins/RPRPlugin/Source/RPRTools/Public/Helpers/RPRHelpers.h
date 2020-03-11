/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "RadeonProRender.h"
#include "Math/mathutils.h"
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "RprTools.h"
#include "Templates/Function.h"
#include "RPRSettings.h"

RPRTOOLS_API RadeonProRender::matrix BuildMatrixNoScale(const struct FTransform &transform, float translationScale = 1.0f);
RPRTOOLS_API RadeonProRender::matrix BuildMatrixWithScale(const struct FTransform &transform, float translationScale = 1.0f);

namespace RPR
{
	/*
	* Returns true if the result is a success
	*/
	RPRTOOLS_API bool		IsResultSuccess(rpr_int Result);

	/*
	* Returns true if the results is a failure
	*/
	RPRTOOLS_API bool		IsResultFailed(rpr_int Result);

	/*
	* Delete a native object.
	* Use the native RPR function to delete object.
	* Assign null to the object after destruction if succeed.
	*/
	RPRTOOLS_API FResult		DeleteObject(void*& Object);

	RPRTOOLS_API FResult		SetObjectName(void* Object, const TCHAR* Name);

	RPRTOOLS_API FResult		SceneClear(RPR::FScene Scene);

	RPRTOOLS_API FResult		RegisterPlugin(const FString& DllPath);

	RPRTOOLS_API bool			AreDevicesCompatible(
		RPR::FPluginId PluginId,
		const FString& RenderCachePath,
		bool bDoWhiteListTest,
		RPR::FCreationFlags DevicesUsed,
		RPR::FCreationFlags& OutDevicesCompatible,
		RPR_TOOLS_OS ToolsOS
	);

	RPRTOOLS_API URPRSettings*	GetSettings();

	namespace RPRMaterial
	{
		using FMaterialNodeFinder = TFunction<bool(RPR::FMaterialNode, int32, const FString&, RPR::EMaterialNodeInputType)>;

		template<typename T>
		RPR::FResult GetNodeInfo(RPR::FMaterialNode MaterialNode, RPR::EMaterialNodeInfo Info, T* OutValue)
		{
			RPR::FResult status;
			TArray<uint8> bufferData;
			uint32 bufferSize;

			status = rprMaterialNodeGetInfo(MaterialNode, (RPR::FMaterialNodeInfo) Info, 0, nullptr, (size_t*) &bufferSize);
			if (RPR::IsResultFailed(status))
			{
				return (status);
			}

			bufferData.AddUninitialized(bufferSize);
			status = rprMaterialNodeGetInfo(MaterialNode, (RPR::FMaterialNodeInfo) Info, bufferSize, bufferData.GetData(), nullptr);
			if (RPR::IsResultSuccess(status))
			{
				FMemory::Memcpy(OutValue, bufferData.GetData(), sizeof(T));
			}
			return (status);
		}

		RPRTOOLS_API RPR::FResult	GetNodeName(RPR::FMaterialNode MaterialNode, FString& OutName);
		RPRTOOLS_API FString		GetNodeName(RPR::FMaterialNode MaterialNode);

		RPRTOOLS_API RPR::FResult GetNodeInputName(RPR::FMaterialNode MaterialNode, int32 InputIndex, FString& OutName);
		RPRTOOLS_API RPR::FResult GetNodeInputType(RPR::FMaterialNode MaterialNode, int32 InputIndex, RPR::EMaterialNodeInputType& OutInputType);
		RPRTOOLS_API RPR::FResult GetNodeInputValue(RPR::FMaterialNode MaterialNode, int32 InputIndex, TArray<uint8>& OutRawDatas);
		RPRTOOLS_API RPR::FResult GetNodeInputInfo(RPR::FMaterialNode MaterialNode, int32 InputIndex, RPR::EMaterialNodeInputInfo Info, TArray<uint8>& OutRawDatas);

		template<typename T>
		RPR::FResult GetNodeInputValue(RPR::FMaterialNode MaterialNode, int32 InputIndex, T& OutValue)
		{
			TArray<uint8> rawDatas;
			RPR::FResult status = GetNodeInputValue(MaterialNode, InputIndex, rawDatas);
			if (RPR::IsResultSuccess(status))
			{
				FMemory::Memcpy(&OutValue, rawDatas.GetData(), sizeof(T));
			}
			return (status);
		}

		template<typename T>
		RPR::FResult GetNodeInputValue(RPR::FMaterialNode MaterialNode, int32 InputIndex, T*& OutValue)
		{
			TArray<uint8> rawDatas;
			RPR::FResult status = GetNodeInputValue(MaterialNode, InputIndex, rawDatas);
			if (RPR::IsResultSuccess(status))
			{
				uint64 address;
				FMemory::Memcpy(&address, rawDatas.GetData(), sizeof(void*)); // Size of a pointer
				OutValue = (void*) address;
			}
			return (status);
		}

		RPRTOOLS_API bool FindInMaterialNode(RPR::FMaterialNode MaterialNode, FMaterialNodeFinder Finder);
		RPRTOOLS_API bool FindFirstImageAvailable(RPR::FContext Context, RPR::FMaterialNode MaterialNode, RPR::FImage& OutImage);
	}

}
