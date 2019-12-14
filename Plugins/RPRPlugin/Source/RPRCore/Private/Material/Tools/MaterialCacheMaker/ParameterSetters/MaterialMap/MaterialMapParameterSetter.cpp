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
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialMap/MaterialMapParameterSetter.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/MaterialContext.h"
#include "Typedefs/RPRTypedefs.h"
#include "RPRCoreModule.h"
#include "Helpers/RPRHelpers.h"
#include "Enums/RPREnums.h"
#include "Constants/RPRMaterialNodeParameterNames.h"

DECLARE_LOG_CATEGORY_CLASS(LogMaterialMapParameterSetter, Log, All)

namespace RPRX
{

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		ApplyTextureParameter(SetterParameters);
	}

	bool FMaterialMapParameterSetter::ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		RPR::FMaterialNode materialNode = nullptr;
		RPR::FMaterialNode imageNode = nullptr;

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		if (materialMap->Texture != nullptr && SetterParameters.ImageManager.IsValid())
		{
			RPR::FImagePtr image;
			RPR::FResult imageNodeCreationResult = CreateImageNodeFromTexture(SetterParameters, image, materialNode, imageNode);

			SetterParameters.Material->AddImage(image);

			if (RPR::IsResultFailed(imageNodeCreationResult))
			{
				UE_LOG(LogRPRCore, Warning,
					TEXT("Cannot create image node texture for '%s'. Error code : %d"),
					*SetterParameters.Property->GetName(),
					imageNodeCreationResult);

				return (false);
			}

			if (imageNode != nullptr)
			{
				ApplyUVSettings(SetterParameters, imageNode);
			}
		}

		SetterParameters.Material->SetMaterialParameterNode(SetterParameters.GetRprxParam(), materialNode);
		return (true);
	}

	RPR::FResult FMaterialMapParameterSetter::CreateImageNodeFromTexture(MaterialParameter::FArgs& SetterParameters,
						RPR::FImagePtr& OutImage, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		RPR::FMaterialNode materialNode = nullptr;
		RPR::FMaterialNode imageNode = nullptr;

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		if (materialMap->Texture == nullptr || !SetterParameters.ImageManager.IsValid())
		{
			OutImage.Reset();
			OutMaterialNode = nullptr;
			OutImageNode = nullptr;
			return RPR_SUCCESS;
		}

		return RPR::FMaterialHelpers::CreateImageNode(
			materialContext.RPRContext,
			materialContext.MaterialSystem,
			*SetterParameters.ImageManager.Get(),
			materialMap->Texture,
			OutImage, OutMaterialNode, OutImageNode);
	}

	bool FMaterialMapParameterSetter::ApplyUVSettings(MaterialParameter::FArgs& SetterParameters, RPR::FMaterialNode ImageMaterialNode)
	{
		RPR::FResult status;

		// Check we are not messing around with an incorrect material node.
		{
			RPR::EMaterialNodeType materialNodeType;
			status = RPR::RPRMaterial::GetNodeInfo(ImageMaterialNode, RPR::EMaterialNodeInfo::Type, &materialNodeType);
			check(materialNodeType == RPR::EMaterialNodeType::ImageTexture);
		}


	#define SET_UV_PARAMETER(Function, ParameterName, Value) \
		status = Function(uvProjectNode, ParameterName, Value); \
		if (RPR::IsResultFailed(status)) \
		{ \
			UE_LOG(LogMaterialMapParameterSetter, Warning, TEXT("Cannot set UV data '%s' for parameter %s"), ParameterName, *SetterParameters.Property->GetName()); \
			return (false); \
		}

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		const FRPRMaterialMapUV& uvSettings = materialMap->UVSettings;

		RPR::FMaterialNode uvProjectNode = nullptr;

		if (uvSettings.UVMode == ETextureUVMode::None)
		{
			if (!CreateSimpleUVNodeData(SetterParameters, uvSettings, uvProjectNode))
			{
				return false;
			}
		}
		else
		{
			RPR::EMaterialNodeType materialNodeType =	(uvSettings.UVMode == ETextureUVMode::Triplanar) ? RPR::EMaterialNodeType::UVTriplanar : RPR::EMaterialNodeType::UVProcedural;
			FString nodeName =							(uvSettings.UVMode == ETextureUVMode::Triplanar) ? TEXT("UV Triplanar") : TEXT("UV Procedural");

			status = RPR::FMaterialHelpers::CreateNode(SetterParameters.MaterialContext.MaterialSystem, materialNodeType, nodeName, uvProjectNode);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogMaterialMapParameterSetter, Warning, TEXT("Cannot create UV node for parameter %s"), *SetterParameters.Property->GetName());
				return (false);
			}

			if (materialNodeType == RPR::EMaterialNodeType::UVProcedural)
			{
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats,	RPR::Constants::MaterialNode::UV::Procedural::Origin,		FVector4(uvSettings.Origin.X, uvSettings.Origin.Y, 0, 1.0f));
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats,	 RPR::Constants::MaterialNode::UV::Procedural::Threshold,	FVector4(uvSettings.Threshold.X, uvSettings.Threshold.Y, uvSettings.Threshold.Z, 1.0f));
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputUInt,	RPR::Constants::MaterialNode::UV::Procedural::UVType,		uvSettings.GetRPRValueFromTextureUVMode());
			}
			else if (materialNodeType == RPR::EMaterialNodeType::UVTriplanar)
			{
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats, RPR::Constants::MaterialNode::UV::Triplanar::Weight, uvSettings.UVWeight);
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats, RPR::Constants::MaterialNode::UV::Triplanar::Offset, FVector4(uvSettings.Origin.X, uvSettings.Origin.Y, 0, 1.0f));
			}

			// Kind of adaptation from UE4 convention to RPR convention
			SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats, RPR::Constants::MaterialNode::UV::XAxis, FVector4(-uvSettings.XAxis.X, uvSettings.XAxis.Z, uvSettings.XAxis.Y, 1.0f));
			SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats, RPR::Constants::MaterialNode::UV::ZAxis, FVector4(-uvSettings.ZAxis.X, uvSettings.ZAxis.Z, uvSettings.ZAxis.Y, 1.0f));
			SET_UV_PARAMETER(RPR::FMaterialHelpers::FMaterialNode::SetInputFloats, RPR::Constants::MaterialNode::UV::UVScale, uvSettings.Scale);
		}

		unsigned int uvInputDataName = uvSettings.UVChannel == 0 ?
			RPR::Constants::MaterialNode::ImageTexture::UV :
			RPR::Constants::MaterialNode::ImageTexture::UV2;

		status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(ImageMaterialNode, uvInputDataName, uvProjectNode);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogMaterialMapParameterSetter, Warning, TEXT("Cannot bind UV node to image node for parameter %s"), *SetterParameters.Property->GetName());
			return (false);
		}

		return (true);

	#undef SET_UV_PARAMETER
	}

	bool FMaterialMapParameterSetter::CreateSimpleUVNodeData(MaterialParameter::FArgs& SetterParameters, const FRPRMaterialMapUV& UVSettings, RPR::FMaterialNode& OutMaterialNode)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		RPR::FResult status;

		// Create lookup node
		RPR::FMaterialNode inputLookupNode;
		RPR::EMaterialNodeLookupValue lookupUVValue = (UVSettings.UVChannel == 0) ? RPR::EMaterialNodeLookupValue::UV : RPR::EMaterialNodeLookupValue::UV1;
		status = RPR::FMaterialHelpers::CreateNode(materialContext.MaterialSystem, RPR::EMaterialNodeType::InputLookup, TEXT("Input Lookup UV"), inputLookupNode); check(status == 0);
		status = RPR::FMaterialHelpers::FMaterialNode::SetInputEnum(inputLookupNode, RPR::Constants::MaterialNode::Lookup::Value, lookupUVValue); check(status == 0);

		// Create add node to offset UV
		RPR::FMaterialNode offsetNode;
		status = RPR::FMaterialHelpers::FArithmeticNode::CreateArithmeticNode(
			materialContext.MaterialSystem,
			RPR::EMaterialNodeArithmeticOperation::Add,
			TEXT("Arithmetic for UV offset - Add"),
			offsetNode);
		check(status == 0);

		status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(offsetNode, RPR::Constants::MaterialNode::Color0, inputLookupNode); check(status == 0);
		status = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(offsetNode, RPR::Constants::MaterialNode::Color1, -UVSettings.Origin.X, -UVSettings.Origin.Y); check(status == 0);

		// Create multiply node to scale UV
		RPR::FMaterialNode uvScaledNode;
		status = RPR::FMaterialHelpers::FArithmeticNode::CreateArithmeticNode(
			materialContext.MaterialSystem,
			RPR::EMaterialNodeArithmeticOperation::Multiply,
			TEXT("Arithmetic for UV scale - Multiply"),
			uvScaledNode);
		check(status == 0);

		status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(uvScaledNode, RPR::Constants::MaterialNode::Color0, offsetNode); check(status == 0);
		status = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(uvScaledNode, RPR::Constants::MaterialNode::Color1, UVSettings.Scale); check(status == 0);

		// Create rotation node to rotate UV
		RPR::FMaterialHelpers::FArithmeticNode::FRotationNode rotationNode;
		status = RPR::FMaterialHelpers::FArithmeticNode::CreateVector2DRotationNode(materialContext.MaterialSystem, TEXT("UV Rotation"), rotationNode); check(status == 0);

		rotationNode.SetInputVector2D(uvScaledNode);
		rotationNode.SetRotationAngle(FMath::DegreesToRadians(UVSettings.Rotation));

		OutMaterialNode = rotationNode.GetOutputNode();
		return true;
	}

}
