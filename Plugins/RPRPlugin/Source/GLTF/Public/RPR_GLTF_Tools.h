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
#include "Typedefs/RPRTypedefs.h"
#include "Typedefs/RPRXTypedefs.h"
#include "RPR_GLTFModule.h"
#include "ProRenderGLTF.h"

namespace RPR
{
	namespace GLTF
	{
		using FStatus = gltf_status;

		RPR_GLTF_API RPR::FResult ImportFromGLFT(
			const FString& Filename,
			RPR::FContext Context,
			RPR::FMaterialSystem MaterialSystem,
			RPRX::FContext RPRContext,
			RPR::FScene& OutScene
		);

		RPR_GLTF_API RPR::FResult ExportToGLTF(
			const FString& Filename,
			RPR::FContext Context,
			RPR::FMaterialSystem MaterialSystem,
			RPRX::FContext RPRContext,
			const TArray<RPR::FScene>& Scenes
		);

		RPR_GLTF_API bool		IsResultSuccess(FStatus status);
		RPR_GLTF_API bool		IsResultFailed(FStatus status);
		RPR_GLTF_API FString	GetStatusText(FStatus status);

		namespace Import
		{
			RPR_GLTF_API FStatus GetShapes(TArray<RPR::FShape>& OutShapes);
			RPR_GLTF_API FStatus GetLights(TArray<RPR::FLight>& OutLights);
			RPR_GLTF_API FStatus GetImages(TArray<RPR::FImage>& OutImages);
			RPR_GLTF_API FStatus GetCameras(TArray<RPR::FCamera>& OutCameras);
			RPR_GLTF_API FStatus GetMaterialX(TArray<RPRX::FMaterial>& OutMaterials);
			RPR_GLTF_API FStatus GetMaterialNodes(TArray<RPR::FMaterialNode>& OutMaterialNodes);
		}

		namespace Group
		{
			RPR_GLTF_API FStatus GetParentGroupFromShape(RPR::FShape Shape, FString& OutGroupName);
			RPR_GLTF_API FStatus GetParentGroupFromGroup(const FString& GroupChild, FString& OutGroupName);

			RPR_GLTF_API RPR::FResult	GetLocalTransform(const FString& GroupName, FTransform& OutTransform);
			RPR_GLTF_API RPR::FResult	GetWorldTransform(const FString& GroupName, FTransform& OutTransform);
		}

	}
};
