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

#include "Material/RPRMaterialHelpers.h"
#include "RadeonProRender.h"
#include "Helpers/RPRHelpers.h"
#include "Logging/LogMacros.h"
#include "RPRCoreModule.h"
#include "Constants/RPRMaterialNodeParameterNames.h"

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
													UTexture2D* Texture, RPR::FImagePtr& OutImage,
													RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		OutMaterialNode = nullptr;
		OutImageNode = nullptr;

		OutImage = ImageManager.LoadImageFromTexture(Texture);
		if (!OutImage.IsValid())
		{
			return (RPR_ERROR_INVALID_IMAGE);
		}

		RPR::FResult	result;

		result = CreateNode(MaterialSystem, EMaterialNodeType::ImageTexture, Texture->GetName(), OutMaterialNode);
		if (IsResultSuccess(result))
		{
			result = RPR::FMaterialHelpers::FMaterialNode::SetInputImageData(OutMaterialNode, RPR::Constants::MaterialNode::ImageTexture::ImageData, OutImage.Get());
		}
		OutImageNode = OutMaterialNode;

		return (result);
	}

	RPR::FResult FMaterialHelpers::CreateNormalMap(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
													UTexture2D* Texture, RPR::FImagePtr& OutImage,
													RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		OutMaterialNode = nullptr;
		OutImageNode = nullptr;

		FString normalMapNodeName = FString::Printf(TEXT("NormalMap_%s"), *Texture->GetName());
		RPR::FResult result = CreateNode(MaterialSystem, EMaterialNodeType::NormalMap, *normalMapNodeName, OutMaterialNode);
		if (IsResultSuccess(result))
		{
			result = CreateImageNode(RPRContext, MaterialSystem, ImageManager, Texture, OutImage, OutMaterialNode, OutImageNode);
			if (IsResultSuccess(result))
			{
				result = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(OutMaterialNode, RPR::Constants::MaterialNode::Color, OutImageNode);
			}
		}

		return result;
	}

	RPR::FResult FMaterialHelpers::CreateBumpMap(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
		UTexture2D* Texture, float BumpScale,
		RPR::FImagePtr& OutImage, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		OutMaterialNode = nullptr;
		OutImageNode = nullptr;

		FString bumpMapNodeName = FString::Printf(TEXT("BumpMap_%s"), *Texture->GetName());
		RPR::FResult result = CreateNode(MaterialSystem, EMaterialNodeType::BumpMap, *bumpMapNodeName, OutMaterialNode);
		if (IsResultFailed(result)) { return result; }

		RPR::FMaterialNode tempNode;
		result = CreateImageNode(RPRContext, MaterialSystem, ImageManager, Texture, OutImage, tempNode, OutImageNode);
		if (IsResultFailed(result)) { return result; }

		result = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(OutMaterialNode, RPR::Constants::MaterialNode::Color, OutImageNode);
		if (IsResultFailed(result)) { return result; }

		result = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(OutMaterialNode, RPR::Constants::MaterialNode::BumpScale, BumpScale);
		return result;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputUInt(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, unsigned int Value)
	{
		RPR::FResult status = rprMaterialNodeSetInputUByKey(MaterialNode, ParameterName, Value);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputU(materialNode=%s:%p, parameterName=%d, value=%d) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			ParameterName,
			Value,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputNode(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, RPR::FMaterialNode InMaterialNode)
	{
		RPR::FResult status = rprMaterialNodeSetInputNByKey(MaterialNode, ParameterName, InMaterialNode);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputN(materialNode=%s:%p, parameterName=%d, materialNode=%s:%p) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			ParameterName,
			*RPR::RPRMaterial::GetNodeName(InMaterialNode),
			InMaterialNode,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputImageData(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, RPR::FImage InImage)
	{
		RPR::FResult status = rprMaterialNodeSetInputImageDataByKey(MaterialNode, ParameterName, InImage);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputImageData(materialNode=%s:%p, parameterName=%d, image=%p) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			ParameterName,
			InImage,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, float x, float y, float z, float w)
	{
		RPR::FResult status = rprMaterialNodeSetInputFByKey(MaterialNode, ParameterName, x, y, z, w);

		UE_LOG(LogRPRCore_Steps, Verbose,
			TEXT("rprMaterialNodeSetInputF(materialNode=%s:%p, parameterName=%d, x=%.2f, y=%.2f, z=%.2f, w=%.2f) -> %d"),
			*RPR::RPRMaterial::GetNodeName(MaterialNode),
			MaterialNode,
			ParameterName,
			x, y, z, w,
			status);

		return status;
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, const FVector& Value, float w)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, Value.Z, w));
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, const FVector4& Value)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, Value.Z, Value.W));
	}

	RPR::FResult FMaterialHelpers::FMaterialNode::SetInputFloats(RPR::FMaterialNode MaterialNode, unsigned int ParameterName, const FVector2D& Value, float z /*= 0.0f*/, float w /*= 0.0f*/)
	{
		return (SetInputFloats(MaterialNode, ParameterName, Value.X, Value.Y, z, w));
	}

	RPR::FResult FMaterialHelpers::FArithmeticNode::CreateArithmeticNode(FMaterialSystem MaterialSystem, RPR::EMaterialNodeArithmeticOperation Operation, RPR::FMaterialNode& OutMaterialNode)
	{
		return CreateArithmeticNode(MaterialSystem, Operation, TEXT("Arithmetic"), OutMaterialNode);
	}

	RPR::FResult FMaterialHelpers::FArithmeticNode::CreateArithmeticNode(FMaterialSystem MaterialSystem, RPR::EMaterialNodeArithmeticOperation Operation, const FString& NodeName, RPR::FMaterialNode& OutMaterialNode)
	{
		RPR::FResult status = RPR::FMaterialHelpers::CreateNode(MaterialSystem, RPR::EMaterialNodeType::Arithmetic, NodeName, OutMaterialNode);

		if (RPR::IsResultFailed(status))
		{
			OutMaterialNode = nullptr;
			return status;
		}

		status = RPR::FMaterialHelpers::FMaterialNode::SetInputEnum(OutMaterialNode, RPR::Constants::MaterialNode::Arithmetic::Operation, Operation);
		return status;
	}

	RPR::FResult FMaterialHelpers::FArithmeticNode::CreateVector2DRotationNode(FMaterialSystem MaterialSystem, const FString& NodeName, FRotationNode& OutRotationNode)
	{
		return FMaterialHelpers::FArithmeticNode::FRotationNode::CreateRotationNode(MaterialSystem, NodeName, OutRotationNode);
	}

	RPR::FResult FMaterialHelpers::FArithmeticNode::FRotationNode::CreateRotationNode(FMaterialSystem MaterialSystem, const FString& NodeName, FRotationNode& OutRotationNode)
	{
		RPR::FResult status;

		status = CreateArithmeticNode(MaterialSystem, EMaterialNodeArithmeticOperation::Dot3, TEXT("Dot3 - A"), OutRotationNode.InputRotationAngleA);
		status = CreateArithmeticNode(MaterialSystem, EMaterialNodeArithmeticOperation::Dot3, TEXT("Dot3 - B"), OutRotationNode.InputRotationAngleB);

		status = CreateArithmeticNode(MaterialSystem, EMaterialNodeArithmeticOperation::Combine, TEXT("Combine"), OutRotationNode.OutputNode);
		status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(OutRotationNode.OutputNode, RPR::Constants::MaterialNode::Color0, OutRotationNode.InputRotationAngleA);
		status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(OutRotationNode.OutputNode, RPR::Constants::MaterialNode::Color1, OutRotationNode.InputRotationAngleB);

		return status;
	}

	void FMaterialHelpers::FArithmeticNode::FRotationNode::SetRotationAngle(float RadAngle)
	{
		RPR::FResult status;

		status = FMaterialHelpers::FMaterialNode::SetInputFloats(InputRotationAngleA, RPR::Constants::MaterialNode::Color1, FMath::Cos(RadAngle), FMath::Sin(RadAngle)); check(status == 0);
		status = FMaterialHelpers::FMaterialNode::SetInputFloats(InputRotationAngleB, RPR::Constants::MaterialNode::Color1, -FMath::Sin(RadAngle), FMath::Cos(RadAngle)); check(status == 0);
	}

	void FMaterialHelpers::FArithmeticNode::FRotationNode::SetInputVector2D(RPR::FMaterialNode MaterialNode)
	{
		RPR::FResult status;

		status = FMaterialHelpers::FMaterialNode::SetInputNode(InputRotationAngleA, RPR::Constants::MaterialNode::Color0, MaterialNode); check(status == 0);
		status = FMaterialHelpers::FMaterialNode::SetInputNode(InputRotationAngleB, RPR::Constants::MaterialNode::Color0, MaterialNode); check(status == 0);
	}

	RPR::FMaterialNode FMaterialHelpers::FArithmeticNode::FRotationNode::GetOutputNode() const
	{
		return OutputNode;
	}

}
