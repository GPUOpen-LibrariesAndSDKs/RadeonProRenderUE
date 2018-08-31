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
#pragma once

#include "Assets/RPRMaterial.h"
#include "Typedefs/RPRTypedefs.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Material/MaterialContext.h"
#include "Material/Tools/MaterialCacheMaker/ParameterArgs.h"

namespace RPRX
{
	DECLARE_DELEGATE_RetVal_FourParams(RPR::FResult, FUberMaterialParametersPropertyVisitor, const FRPRUberMaterialParameters&, UScriptStruct*, UProperty*, RPR::FRPRXMaterial&)

	class FMaterialCacheMaker
	{
	public:

		FMaterialCacheMaker(RPR::FMaterialContext InMaterialContent, const URPRMaterial* InRPRMaterial);

		bool	CacheUberMaterial(RPR::FRPRXMaterial& OutMaterial);
		bool	UpdateUberMaterialParameters(RPR::FRPRXMaterial& InOutMaterial);

	private:

		RPR::FResult	BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, RPR::FRPRXMaterial& OutMaterial);
		RPR::FResult	ApplyUberMaterialParameter(const FRPRUberMaterialParameters& Parameters, UScriptStruct* ParametersStruct,
													UProperty* ParameterProperty, RPR::FRPRXMaterial& InOutMaterial);

	private:

		RPR::FMaterialContext	MaterialContext;
		const URPRMaterial*		RPRMaterial;
	};

}

