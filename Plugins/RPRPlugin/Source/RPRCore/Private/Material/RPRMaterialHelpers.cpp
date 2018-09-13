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
#include "RPRCoreModule.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialHelpers, Log, All)

namespace RPR
{
	const TCHAR* FMaterialHelpers::ImageDataInputName(TEXT("data"));

	FResult	FMaterialHelpers::CreateNode(FMaterialSystem MaterialSystem, RPR::EMaterialNodeType NodeType, RPR::FMaterialNode& OutMaterialNode)
	{
		FResult status = rprMaterialSystemCreateNode(MaterialSystem, (RPR::FMaterialNodeType) NodeType, &OutMaterialNode);
		
		UE_LOG(LogRPRCore_Steps, Verbose, 
			TEXT("rprMaterialSystemCreateNode(materialSystem=%p, nodeType=%d) -> status=%d, materialNode=%p"), 
			MaterialSystem, (uint32) NodeType, status, OutMaterialNode);

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialHelpers, Warning, TEXT("Couldn't create RPR material node (%#08)"), status);
		}

		return (status);
	}

	RPR::FResult FMaterialHelpers::CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, const FString& NodeName, RPR::FMaterialNode& OutMaterialNode)
	{
		FResult status = CreateNode(MaterialSystem, NodeType, OutMaterialNode);
		if (IsResultSuccess(status) && OutMaterialNode != nullptr)
		{
			status = RPR::SetObjectName(OutMaterialNode, *NodeName);
		}
		return status;
	}

	RPR::FResult FMaterialHelpers::DeleteNode(RPR::FMaterialNode& MaterialNode)
	{
		UE_LOG(LogRPRCore_Steps, Verbose, TEXT("Delete material node %s:%p"), *RPR::RPRMaterial::GetNodeName(MaterialNode), MaterialNode);

		FResult result = RPR::DeleteObject(MaterialNode);
		MaterialNode = nullptr;
		return (result);
	}

	RPR::FResult FMaterialHelpers::CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
															UTexture2D* Texture, RPR::FImageManager::EImageType ImageType, 
															RPR::FImagePtr& OutImage, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		OutMaterialNode = nullptr;
		OutImageNode = nullptr;

		OutImage = ImageManager.LoadImageFromTexture(Texture);
		if (!OutImage.IsValid())
		{
			return (RPR_ERROR_INVALID_IMAGE);
		}

		RPR::FResult	result;
		if (ImageType == RPR::FImageManager::EImageType::NormalMap)
		{
			FString normalMapNodeName = FString::Printf(TEXT("NormalMap_%s"), *Texture->GetName());
			result = CreateNode(MaterialSystem, EMaterialNodeType::NormalMap, *normalMapNodeName, OutMaterialNode);
			if (IsResultSuccess(result))
			{
				result = CreateNode(MaterialSystem, EMaterialNodeType::ImageTexture, Texture->GetName(), OutImageNode);
				if (IsResultSuccess(result))
				{
					result = rprMaterialNodeSetInputImageData(OutImageNode, TCHAR_TO_ANSI(ImageDataInputName), OutImage.Get());
					if (IsResultSuccess(result))
					{
						result = rprMaterialNodeSetInputN(OutMaterialNode, "color", OutImageNode);
					}
				}
			}
		}
		else
		{
			result = CreateNode(MaterialSystem, EMaterialNodeType::ImageTexture, Texture->GetName(), OutMaterialNode);
			if (IsResultSuccess(result))
			{
				result = rprMaterialNodeSetInputImageData(OutMaterialNode, TCHAR_TO_ANSI(ImageDataInputName), OutImage.Get());
			}
			OutImageNode = OutMaterialNode;
		}

		return (result);
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputUInt(RPR::FMaterialNode MaterialNode, const FString& ParameterName, uint8 Value)
	{
		RPR::FResult status = rprMaterialNodeSetInputU(MaterialNode, TCHAR_TO_ANSI(*ParameterName), Value);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputU(materialNode=%s:%p, parameterName=%s, value=%d) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			*ParameterName,
			Value,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputNode(RPR::FMaterialNode MaterialNode, const FString& ParameterName, RPR::FMaterialNode InMaterialNode)
	{
		RPR::FResult status = rprMaterialNodeSetInputN(MaterialNode, TCHAR_TO_ANSI(*ParameterName), InMaterialNode);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputN(materialNode=%s:%p, parameterName=%s, materialNode=%s:%p) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			*ParameterName,
			*RPR::RPRMaterial::GetNodeName(InMaterialNode),
			InMaterialNode,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, float x, float y, float z, float w)
	{
		RPR::FResult status = rprMaterialNodeSetInputF(MaterialNode, TCHAR_TO_ANSI(*ParameterName), x, y, z, w);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputF(materialNode=%s:%p, parameterName=%s, x=%.2f, y=%.2f, z=%.2f, w=%.2f) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			*ParameterName,
			x, y, z, w,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector& Value, float w)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, Value.Z, w));
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector4& Value)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, Value.Z, Value.W));
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, const FString& ParameterName, const FVector2D& Value, float z /*= 0.0f*/, float w /*= 0.0f*/)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, z, w));
	}
}
