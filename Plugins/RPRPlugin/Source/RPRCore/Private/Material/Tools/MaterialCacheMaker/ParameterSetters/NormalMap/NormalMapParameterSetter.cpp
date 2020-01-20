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
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/NormalMap/NormalMapParameterSetter.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/MaterialContext.h"
#include "Typedefs/RPRTypedefs.h"
#include "RPRCoreModule.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialNormalMap.h"

namespace RPRX
{

	RPR::FResult FNormalMapParameterSetter::CreateImageNodeFromTexture(MaterialParameter::FArgs& SetterParameters, RPR::FImagePtr& OutImage, RPR::FMaterialNode& OutMaterialNode, RPR::FMaterialNode& OutImageNode)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		RPR::FMaterialNode materialNode = nullptr;
		RPR::FMaterialNode imageNode = nullptr;

		const FRPRMaterialNormalMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialNormalMap>();
		if (materialMap->Texture == nullptr || !SetterParameters.ImageManager.IsValid())
		{
			OutImage.Reset();
			OutMaterialNode = nullptr;
			OutImageNode = nullptr;
			return RPR_SUCCESS;
		}

		RPR::FResult status = RPR_SUCCESS;

		switch (materialMap->Mode)
		{
			case ENormalMapMode::Bump:
			{
				status = RPR::FMaterialHelpers::CreateBumpMap(
					materialContext.RPRContext,
					materialContext.MaterialSystem,
					*SetterParameters.ImageManager.Get(),
					materialMap->Texture, materialMap->BumpScale,
					OutImage, OutMaterialNode, OutImageNode);
			}
			break;

			case ENormalMapMode::Normal:
			default:
			{
				status = RPR::FMaterialHelpers::CreateNormalMap(
					materialContext.RPRContext,
					materialContext.MaterialSystem,
					*SetterParameters.ImageManager.Get(),
					materialMap->Texture,
					OutImage, OutMaterialNode, OutImageNode);
			}
			break;
		}

		return status;
	}

}
