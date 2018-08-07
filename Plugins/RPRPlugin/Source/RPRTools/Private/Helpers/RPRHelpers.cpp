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

#include "Helpers/RPRHelpers.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"
#include "RprSupport.h"
#include "RprTools.h"
#include "HAL/UnrealMemory.h"
#include "Helpers/RPRErrorsHelpers.h"
#include "Helpers/RPRXMaterialHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRHelpers, Log, All);

static const float RPR_PI = 3.14159265f;

// Probably not the ideal way of converting UE4 matrices to RPR
// If you find a better way, have fun :)
RadeonProRender::matrix	BuildMatrixNoScale(const FTransform &transform)
{
	const FVector	&position = transform.GetLocation() * 0.1f;
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

RadeonProRender::matrix	BuildMatrixWithScale(const FTransform &transform)
{
	const FVector	&position = transform.GetLocation() * 0.1f;
	const FVector	&scale = transform.GetScale3D();
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::float3		rprScale(scale.X, scale.Z, scale.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion) * RadeonProRender::scale(rprScale);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

namespace RPR
{

	bool IsResultSuccess(rpr_int Result)
	{
		return (Result == RPR_SUCCESS);
	}

	bool IsResultFailed(rpr_int Result)
	{
		return (!IsResultSuccess(Result));
	}

	FResult DeleteObject(void* Object)
	{
		return rprObjectDelete(Object);
	}

	FResult SceneDetachShape(FScene Scene, FShape Shape)
	{
		return rprSceneDetachShape(Scene, Shape);
	}

	FResult ShapeSetMaterial(FShape Shape, RPR::FMaterialNode MaterialNode)
	{
		return (rprShapeSetMaterial(Shape, MaterialNode));
	}

	FResult SceneClear(FScene Scene)
	{
		return (rprSceneClear(Scene));
	}

	FResult RegisterPlugin(const FString& DllPath)
	{
		return rprRegisterPlugin(TCHAR_TO_ANSI(*DllPath));
	}

	bool AreDevicesCompatible(FPluginId PluginId, const FString& RenderCachePath, bool bDoWhiteListTest, FCreationFlags DevicesUsed, FCreationFlags& OutDevicesCompatible, RPR_TOOLS_OS ToolsOS)
	{
		rprAreDevicesCompatible(PluginId, TCHAR_TO_ANSI(*RenderCachePath), bDoWhiteListTest, DevicesUsed, &OutDevicesCompatible, ToolsOS);
		return (OutDevicesCompatible > 0);
	}

	namespace RPRMaterial
	{
		RPR::FResult GetNodeInputName(RPR::FMaterialNode MaterialNode, int32 InputIndex, FString& OutName)
		{
			TArray<uint8> rawDatas;
			RPR::FResult status = GetNodeInputInfo(MaterialNode, InputIndex, EMaterialNodeInputInfo::NameString, rawDatas);
			if (RPR::IsResultSuccess(status))
			{
				OutName = FString((char*) rawDatas.GetData());
			}
			return (status);
		}

		RPR::FResult GetNodeInputType(RPR::FMaterialNode MaterialNode, int32 InputIndex, RPR::EMaterialNodeInputType& OutInputType)
		{
			TArray<uint8> rawDatas;
			RPR::FResult result = GetNodeInputInfo(MaterialNode, InputIndex, EMaterialNodeInputInfo::Type, rawDatas);
			FMemory::Memcpy(&OutInputType, rawDatas.GetData(), sizeof(RPR::EMaterialNodeInputType));
			return (result);
		}

		RPR::FResult GetNodeInputValue(RPR::FMaterialNode MaterialNode, int32 InputIndex, TArray<uint8>& OutRawDatas)
		{
			return (GetNodeInputInfo(MaterialNode, InputIndex, EMaterialNodeInputInfo::Value, OutRawDatas));
		}

		RPR::FResult GetNodeInputInfo(RPR::FMaterialNode MaterialNode, int32 InputIndex, RPR::EMaterialNodeInputInfo Info, TArray<uint8>& OutRawDatas)
		{
			RPR::FResult status;

			uint32 bufferSize;
			status = rprMaterialNodeGetInputInfo(MaterialNode, InputIndex, (RPR::FMaterialNodeInputInfo) Info, 0, nullptr, (size_t*) &bufferSize);
			if (RPR::IsResultFailed(status) || bufferSize <= 1)
			{
				OutRawDatas.Empty();
				return (status);
			}

			OutRawDatas.Empty(bufferSize);
			OutRawDatas.AddUninitialized(bufferSize);
			status = rprMaterialNodeGetInputInfo(MaterialNode, InputIndex, (RPR::FMaterialNodeInputInfo) Info, bufferSize, OutRawDatas.GetData(), nullptr);
			return (status);
		}

		
		bool FindInMaterialNode(RPR::FContext Context, RPR::FMaterialNode MaterialNode, FMaterialNodeFinder Finder)
		{
			RPR::FResult status;

			uint64 numInputs;
			status = RPR::RPRMaterial::GetNodeInfo(MaterialNode, RPR::EMaterialNodeInfo::InputCount, &numInputs);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRHelpers, Warning, TEXT("Cannot get node input count (error code %d)"), status);
				return (false);
			}

			for (int32 inputIndex = 0; inputIndex < numInputs; ++inputIndex)
			{
				FString name;
				status = RPR::RPRMaterial::GetNodeInputName(MaterialNode, inputIndex, name);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRHelpers, Warning, TEXT("Cannot get node input name (error code %d)"), status);
					return (false);
				}

				RPR::EMaterialNodeInputType inputType;
				status = RPR::RPRMaterial::GetNodeInputType(MaterialNode, inputIndex, inputType);
				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRHelpers, Warning, TEXT("%s -> Cannot get node input type (error code %d)"), *name, status);
					return (false);
				}

				if (Finder(MaterialNode, inputIndex, name, inputType))
				{
					return (true);
				}

				if (inputType == EMaterialNodeInputType::Node)
				{
					RPR::FMaterialNode childNode = nullptr;
					status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, inputIndex, childNode);

					if (childNode != nullptr && FindInMaterialNode(Context, childNode, Finder))
					{
						return (true);
					}
				}
			}

			return (false);
		}

		bool FindFirstImageAvailable(RPR::FContext Context, RPR::FMaterialNode MaterialNode, RPR::FImage& OutImage)
		{			
			FMaterialNodeFinder finder = 
				[&Context, &OutImage] (RPR::FMaterialNode node, int32 inputIndex, const FString& inputName, RPR::EMaterialNodeInputType inputType)
			{
				if (inputName.Compare(TEXT("data"), ESearchCase::IgnoreCase) == 0 && inputType == EMaterialNodeInputType::Image)
				{
					RPR::FResult status = RPR::RPRMaterial::GetNodeInputValue(node, inputIndex, OutImage);
					if (RPR::IsResultFailed(status))
					{
						OutImage = nullptr;
						UE_LOG(LogRPRHelpers, Warning, TEXT("%s -> Cannot get node input image"), *inputName);
						RPR::Error::LogLastError(Context);
						return (false);
					}
					return (true);
				}
				return (false);
			};

			return (FindInMaterialNode(Context, MaterialNode, finder));
		}

	} // namespace RPRMaterial

} // namespace RPR
