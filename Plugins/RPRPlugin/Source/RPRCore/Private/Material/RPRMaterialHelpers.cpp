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


	RPR::FResult FMaterialHelpers::CreateImageNode(FImageNodeCreationParameters& CreationParameters, RPR::FMaterialNode& OutMaterialNode)
	{
		RPR::FMaterialNode imageNode;
		return CreateImageNode(CreationParameters, OutMaterialNode, imageNode);
	}

	RPR::FResult FMaterialHelpers::CreateImageNode(FImageNodeCreationParameters& CreationParameters, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		OutMaterialNode = nullptr;
		OutImageNode = nullptr;

		RPR::FImage	image = CreationParameters.ImageManager->LoadImageFromTexture(CreationParameters.Texture, CreationParameters.ImageType);
		if (image == nullptr)
		{
			return (RPR_ERROR_INVALID_IMAGE);
		}

		RPR::FResult	result;
		if (CreationParameters.ImageType == RPR::FImageManager::EImageType::NormalMap)
		{
			result = CreateNode(CreationParameters.MaterialSystem, EMaterialNodeType::NormalMap, OutMaterialNode);
			if (IsResultSuccess(result))
			{
				result = CreateNode(CreationParameters.MaterialSystem, EMaterialNodeType::ImageTexture, OutImageNode);
				if (IsResultSuccess(result))
				{
					result = rprMaterialNodeSetInputImageData(OutImageNode, TCHAR_TO_ANSI(RPR::FMaterialHelpers::ImageDataInputName), image);
					if (IsResultSuccess(result))
					{
						result = rprMaterialNodeSetInputN(OutMaterialNode, "color", OutImageNode);
					}
				}
			}
		}
		else
		{
			result = CreateNode(CreationParameters.MaterialSystem, EMaterialNodeType::ImageTexture, OutImageNode);
			if (IsResultSuccess(result))
			{
				result = rprMaterialNodeSetInputImageData(OutImageNode, TCHAR_TO_ANSI(RPR::FMaterialHelpers::ImageDataInputName), image);
				OutMaterialNode = OutImageNode;
			}
		}

		return (result);
	}

	RPR::FResult FMaterialHelpers::FNode::SetInputUInt(RPR::FMaterialNode MaterialNode, const FString& ParameterName, uint8 Value)
	{
		return (rprMaterialNodeSetInputU(MaterialNode, TCHAR_TO_ANSI(*ParameterName), Value));
	}

	RPR::FResult FMaterialHelpers::FNode::SetInputNode(RPR::FMaterialNode MaterialNode, const FString& ParameterName, RPR::FMaterialNode InMaterialNode)
	{
		return (rprMaterialNodeSetInputN(MaterialNode, TCHAR_TO_ANSI(*ParameterName), InMaterialNode));
	}

	RPR::FResult FMaterialHelpers::FNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, float x, float y, float z, float w)
	{
		return (rprMaterialNodeSetInputF(MaterialNode, TCHAR_TO_ANSI(*ParameterName), x, y, z, w));
	}

	RPR::FResult FMaterialHelpers::FNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector& Value, float w)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, Value.Z, w));
	}

	RPR::FResult FMaterialHelpers::FNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector4& Value)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, Value.Z, Value.W));
	}

	RPR::FResult FMaterialHelpers::FNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector2D& Value, float z /*= 0.0f*/, float w /*= 0.0f*/)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, z, w));
	}

} // RPR
