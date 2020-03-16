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
#include "RPRToolsModule.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"

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
		RPRTOOLS_API FResult		SetScene(FContext Context, FScene Scene);
		RPRTOOLS_API FResult		SetAOV(FContext Context, RPR::EAOV AOV, FFrameBuffer& FrameBuffer);
		RPRTOOLS_API FResult        UnSetAOV(FContext Context, RPR::EAOV AOV);
		RPRTOOLS_API FResult		Render(FContext Context);
		RPRTOOLS_API FResult		ResolveFrameBuffer(FContext Context, FFrameBuffer& SrcFrameBuffer, FFrameBuffer& DstFrameBuffer, bool bNormalizeOnly = false);

		RPRTOOLS_API FResult		CreateInstance(FContext Context, RPR::FShape Shape, RPR::FShape& OutShapeInstance);
		RPRTOOLS_API FResult		CreateInstance(FContext Context, RPR::FShape Shape, const FString& InstanceName, RPR::FShape& OutShapeInstance);

		RPRTOOLS_API FResult		CreateMesh(FContext Context, const TCHAR* MeshName,
										const TArray<FVector>& Vertices, const TArray<FVector>& Normals, const TArray<uint32>& Indices,
										const TArray<FVector2D>& Texcoords, const TArray<uint32>& NumFaceVertices, FShape& OutMesh);

		namespace MaterialSystem
		{
			RPRTOOLS_API FResult	Create(RPR::FContext Context, RPR::FMaterialSystemType Type, RPR::FMaterialSystem& OutMaterialSystem);
		}
	}
}
