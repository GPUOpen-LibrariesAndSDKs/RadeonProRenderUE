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

#include "Helpers/RPRHelpers.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"
#include "RprTools.h"
#include "HAL/UnrealMemory.h"
#include "Helpers/RPRErrorsHelpers.h"
#include "Helpers/GenericGetInfo.h"
#include "Helpers/RPRShapeHelpers.h"

#include <stdexcept>

DEFINE_LOG_CATEGORY_STATIC(LogRPRHelpers, Log, All);

static const float RPR_PI = 3.14159265f;

// Probably not the ideal way of converting UE4 matrices to RPR
// If you find a better way, have fun :)
RadeonProRender::matrix	BuildMatrixNoScale(const FTransform &transform, float translationScale)
{
	const FVector	position = transform.GetLocation() * translationScale;
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

RadeonProRender::matrix	BuildMatrixWithScale(const FTransform &transform, float translationScale)
{
	const FVector	&position = transform.GetLocation() * translationScale;
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
		return !IsResultSuccess(Result);
	}

	FResult DeleteObject(void*& Object)
	{
		FResult status = rprObjectDelete(Object);
		UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprObjectDelete(object=%p) -> %d"), Object, status);
		if (IsResultSuccess(status))
		{
			Object = nullptr;
		}
		return status;
	}

	FResult SetObjectName(void* Object, const TCHAR* Name)
	{
		FResult status = rprObjectSetName(Object, TCHAR_TO_ANSI(Name));
		UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprSetObjectName(object=%p, name=%s) -> %d"), Object, Name, status);
		return status;
	}

	FResult SceneClear(FScene Scene)
	{
		return (rprSceneClear(Scene));
	}

	FResult RegisterPlugin(const FString& DllPath)
	{
		return rprRegisterPlugin(TCHAR_TO_UTF8(*DllPath));
	}

	bool AreDevicesCompatible(FPluginId PluginId, const FString& RenderCachePath, bool bDoWhiteListTest, FCreationFlags DevicesUsed, FCreationFlags& OutDevicesCompatible, RPR_TOOLS_OS ToolsOS)
	{
		rprAreDevicesCompatible(PluginId, TCHAR_TO_ANSI(*RenderCachePath), bDoWhiteListTest, DevicesUsed, &OutDevicesCompatible, ToolsOS);
		return (OutDevicesCompatible > 0);
	}

	URPRSettings* GetSettings()
	{
		URPRSettings* settings = GetMutableDefault<URPRSettings>();
		check(settings != nullptr);

		return settings;
	}

	namespace RPRMaterial
	{

		RPR::FResult GetNodeName(RPR::FMaterialNode MaterialNode, FString& OutName)
		{
			check(MaterialNode);
			return RPR::Generic::GetObjectName(rprMaterialNodeGetInfo, MaterialNode, OutName);
		}

		FString GetNodeName(RPR::FMaterialNode MaterialNode)
		{
			if (MaterialNode == nullptr)
			{
				return TEXT("null");
			}

			FString name;
			RPR::FResult status = GetNodeName(MaterialNode, name);
			if (RPR::IsResultFailed(status))
			{
				name = FString::Printf(TEXT("[Unkown:%p]"), MaterialNode);
			}
			else if (name.IsEmpty())
			{
				name = FString::Printf(TEXT("[Undefined:%p]"), MaterialNode);
			}
			return name;
		}

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

		bool IsMaterialNode(void* Object)
		{
			RPR::FMaterialNodeType materialNodeType;
			RPR::FResult status = RPR::RPRMaterial::GetNodeInfo(Object, RPR::EMaterialNodeInfo::Type, &materialNodeType);
			const bool bIsMaterialNodeValid = (RPR::IsResultSuccess(status) && materialNodeType != 0);
			return bIsMaterialNodeValid;
		}

		bool FindInMaterialNode(RPR::FMaterialNode MaterialNode, FMaterialNodeFinder Finder)
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

					if (childNode != nullptr && FindInMaterialNode(childNode, Finder))
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

			return (FindInMaterialNode(MaterialNode, finder));
		}

	} // namespace RPRMaterial

} // namespace RPR
