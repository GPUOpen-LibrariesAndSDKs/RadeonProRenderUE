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
#include "Material/RPRMaterialHelpers.h"
#include "RadeonProRender.h"
#include "Helpers/RPRHelpers.h"
#include "Logging/LogMacros.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialHelpers, Log, All)

namespace RPR
{
	const TCHAR* FMaterialHelpers::ImageDataInputName(TEXT("data"));

	FResult	FMaterialHelpers::CreateNode(FMaterialSystem MaterialSystem, RPR::EMaterialNodeType NodeType, RPR::FMaterialNode& OutMaterialNode)
	{
		FResult result = rprMaterialSystemCreateNode(MaterialSystem, (RPR::FMaterialNodeType) NodeType, &OutMaterialNode);

		if (IsResultFailed(result))
		{
			UE_LOG(LogRPRMaterialHelpers, Warning, TEXT("Couldn't create RPR material node (%#08)"), result);
		}

		return (result);
	}

	RPR::FResult FMaterialHelpers::DeleteNode(RPR::FMaterialNode& MaterialNode)
	{
		FResult result = rprObjectDelete(MaterialNode);
		MaterialNode = nullptr;
		return (result);
	}

	RPR::FResult FMaterialHelpers::CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
															UTexture2D* Texture, RPR::FMaterialNode& OutMaterialNode)
	{
		OutMaterialNode = nullptr;

		RPR::FImage image = ImageManager.LoadImageFromTexture(Texture);
		if (image == nullptr)
		{
			return (RPR_ERROR_INVALID_IMAGE);
		}

		RPR::FResult result = CreateNode(MaterialSystem, EMaterialNodeType::ImageTexture, OutMaterialNode);
		if (IsResultSuccess(result))
		{
			result = rprMaterialNodeSetInputImageData(OutMaterialNode, TCHAR_TO_ANSI(ImageDataInputName), image);
		}

		return (result);
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputF(RPR::FMaterialNode MaterialNode, const FString& ParameterName, float x, float y, float z, float w)
	{
		return (rprMaterialNodeSetInputF(MaterialNode, TCHAR_TO_ANSI(*ParameterName), x, y, z, w));
	}

}
