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
#include "RPRCoreModule.h"

namespace RPRX
{

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		UE_LOG(LogRPRCore_Steps, Verbose, TEXT("[%s] %s -> Set texture : %s"),
			*SetterParameters.OwnerMaterial->GetName(),
			*SetterParameters.Property->GetName(),
			materialMap->Texture != nullptr ? *materialMap->Texture->GetName() : TEXT("None"));

		ApplyTextureParameter(SetterParameters);
	}

	bool FMaterialMapParameterSetter::ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		RPR::FMaterialNode imageMaterialNode = nullptr;

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		if (materialMap->Texture != nullptr && SetterParameters.ImageManager.IsValid())
		{
			RPR::FResult imageNodeCreationResult = RPR::FMaterialHelpers::CreateImageNode(
				materialContext.RPRContext,
				materialContext.MaterialSystem,
				*SetterParameters.ImageManager.Get(),
				materialMap->Texture,
				RPR::FImageManager::EImageType::Standard,
                imageMaterialNode
			);

			// TODO : Bind image
			//SetterParameters.Material.AddImage();

			if (RPR::IsResultFailed(imageNodeCreationResult))
			{
				return (false);
			}
		}

		FMaterialHelpers::SetMaterialParameterNode(
			materialContext.RPRXContext,
			SetterParameters.Material.GetMaterial(),
			SetterParameters.GetRprxParam(),
			imageMaterialNode
		);

		return (true);
	}
}
