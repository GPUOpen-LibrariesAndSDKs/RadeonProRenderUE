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
