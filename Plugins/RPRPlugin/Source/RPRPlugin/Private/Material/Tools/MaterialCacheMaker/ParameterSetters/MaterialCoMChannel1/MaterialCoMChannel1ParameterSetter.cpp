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
#include "MaterialCoMChannel1ParameterSetter.h"
#include "RPRMaterialCoMChannel1.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialCoMChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialCoMChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialCoMChannel1>();

		if (materialMap->Mode == ERPRMaterialMapMode::Texture)
		{
			if (materialMap->Texture != nullptr)
			{
				ApplyTextureParameter(SetterParameters);
			}
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

			switch (materialMap->RPRInterpretationMode)
			{
			case ERPRMCoMapC1InterpretationMode::AsFloat:
				FMaterialHelpers::SetMaterialParameterFloat(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					materialMap->Constant
				);
				break;

			case ERPRMCoMapC1InterpretationMode::AsFloat4:
				FMaterialHelpers::SetMaterialParameterFloats(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					materialMap->Constant,
					materialMap->Constant,
					materialMap->Constant,
					materialMap->Constant
				);
				break;

			default:
				break;
			}
		}
	}

}
