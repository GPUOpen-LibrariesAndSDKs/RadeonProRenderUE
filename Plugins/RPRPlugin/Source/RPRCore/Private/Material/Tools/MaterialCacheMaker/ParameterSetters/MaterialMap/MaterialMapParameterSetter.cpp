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
#include "Helpers/RPRXMaterialHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogMaterialMapParameterSetter, Log, All)

namespace RPRX
{

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		ApplyTextureParameter(SetterParameters);
	}

	bool FMaterialMapParameterSetter::ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		RPR::FMaterialNode materialNode = nullptr;

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		if (materialMap->Texture != nullptr && SetterParameters.ImageManager.IsValid())
		{
			RPR::FMaterialHelpers::FImageNodeCreationParameters imageCreationParameters;
			{
				imageCreationParameters.Context = materialContext.RPRContext;
				imageCreationParameters.MaterialSystem = materialContext.MaterialSystem;
				imageCreationParameters.ImageType = GetImageType();
				imageCreationParameters.Texture = materialMap->Texture;
				imageCreationParameters.ImageManager = SetterParameters.ImageManager;
			}

			RPR::FMaterialNode imageMaterialNode = nullptr;
			RPR::FResult imageNodeCreationResult = RPR::FMaterialHelpers::CreateImageNode(
				imageCreationParameters, materialNode, imageMaterialNode
			);

			if (RPR::IsResultFailed(imageNodeCreationResult))
			{
				return (false);
			}

			ApplyUVSettings(SetterParameters, imageMaterialNode);
		}

		FMaterialHelpers::SetMaterialParameterNode(
			materialContext.RPRXContext,
			SetterParameters.Material,
			SetterParameters.GetRprxParam(),
			materialNode
		);

		return (true);
	}

	RPR::FImageManager::EImageType FMaterialMapParameterSetter::GetImageType() const
	{
		return RPR::FImageManager::EImageType::Standard;
	}

	bool FMaterialMapParameterSetter::ApplyUVSettings(MaterialParameter::FArgs& SetterParameters, RPR::FMaterialNode ImageMaterialNode)
	{

	#define SET_UV_PARAMETER(Function, ParameterName, Value) \
		status = Function(uvProjectNode, ParameterName, Value); \
		if (RPR::IsResultFailed(status)) \
		{ \
			UE_LOG(LogMaterialMapParameterSetter, Warning, TEXT("Cannot set UV data '%s' for parameter %s"), ParameterName, *SetterParameters.Property->GetName()); \
			return (false); \
		}

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();

		RPR::EMaterialNodeType materialNodeType = 
			(materialMap->UVMode == ETextureUVMode::Triplanar) ? RPR::EMaterialNodeType::UVTriplanar : RPR::EMaterialNodeType::UVProcedural;

		RPR::FMaterialNode uvProjectNode;
		RPR::FResult status = RPR::FMaterialHelpers::CreateNode(SetterParameters.MaterialContext.MaterialSystem, materialNodeType, uvProjectNode);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogMaterialMapParameterSetter, Warning, TEXT("Cannot create UV node for parameter %s"), *SetterParameters.Property->GetName());
			return (false);
		}

		if (materialMap->UVMode != ETextureUVMode::None)
		{

			if (materialNodeType == RPR::EMaterialNodeType::UVProcedural)
			{
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("origin"), FVector4(materialMap->Origin.X, materialMap->Origin.Y, 0, 1.0f));
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("threshold"), FVector4(materialMap->Threshold.X, materialMap->Threshold.Y, materialMap->Threshold.Z, 1.0f));
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputUInt, TEXT("uv_type"), materialMap->GetRPRValueFromTextureUVMode());
			}
			else if (materialNodeType == RPR::EMaterialNodeType::UVTriplanar)
			{
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("weight"), materialMap->UVWeight);
				SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("offset"), FVector4(materialMap->Origin.X, materialMap->Origin.Y, 0, 1.0f));
			}
		}
		
		SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("xaxis"), FVector4(materialMap->XAxis.X, materialMap->XAxis.Y, materialMap->XAxis.Z, 1.0f));
		SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("zaxis"), FVector4(materialMap->ZAxis.X, materialMap->ZAxis.Y, materialMap->ZAxis.Z, 1.0f));
		SET_UV_PARAMETER(RPR::FMaterialHelpers::FNode::SetInputFloats, TEXT("uv_scale"), materialMap->Scale);

		status = RPR::FMaterialHelpers::FNode::SetInputNode(ImageMaterialNode, TEXT("uv"), uvProjectNode);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogMaterialMapParameterSetter, Warning, TEXT("Cannot bind UV node to image node for parameter %s"), *SetterParameters.Property->GetName());
			return (false);
		}

		return (true);

	#undef SET_UV_PARAMETER
	}

}
