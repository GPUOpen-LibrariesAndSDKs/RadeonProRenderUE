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
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoM/MaterialCoMParameterSetter.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/MaterialContext.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "RPRCoreModule.h"

namespace RPRX
{

	void FMaterialCoMParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialCoM* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialCoM>();

		if (materialMap->Mode == ERPRMaterialMapMode::Texture)
		{
			UE_LOG(LogRPRCore_Steps, Verbose, TEXT("[%s] %s -> Set texture : %s"), 
				*SetterParameters.OwnerMaterial->GetName(),
				*SetterParameters.Property->GetName(),
				materialMap->Texture != nullptr ? *materialMap->Texture->GetName() : TEXT("None"));

			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			UE_LOG(LogRPRCore_Steps, Verbose, TEXT("[%s] %s -> Set constant value : %s"),
				*SetterParameters.OwnerMaterial->GetName(),
				*SetterParameters.Property->GetName(),
				*materialMap->Constant.ToString());

			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

			SetterParameters.Material->SetMaterialParameterColor(SetterParameters.GetRprxParam(), materialMap->Constant);
		}
	}

}
