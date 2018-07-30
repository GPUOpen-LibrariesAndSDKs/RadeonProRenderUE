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

#include "RadeonProRender.h"
#include "Math/mathutils.h"
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "RprTools.h"
#include "Templates/Function.h"
#include "Typedefs/RPRXTypedefs.h"

RPRTOOLS_API RadeonProRender::matrix BuildMatrixNoScale(const struct FTransform &transform);
RPRTOOLS_API RadeonProRender::matrix BuildMatrixWithScale(const struct FTransform &transform);

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
	*/
	RPRTOOLS_API FResult		DeleteObject(void* Object);

	RPRTOOLS_API FResult		SceneDetachShape(FScene Scene, FShape Shape);

	RPRTOOLS_API FResult		ShapeSetMaterial(FShape Shape, RPR::FMaterialNode MaterialNode);

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

		RPRTOOLS_API bool FindInMaterialNode(RPR::FContext Context, RPR::FMaterialNode MaterialNode, FMaterialNodeFinder Finder);
		RPRTOOLS_API bool FindFirstImageAvailable(RPR::FContext Context, RPR::FMaterialNode MaterialNode, RPR::FImage& OutImage);
	}

}
