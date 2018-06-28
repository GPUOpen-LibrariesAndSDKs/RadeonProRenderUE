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

#include "RPRMaterial.h"
#include "RPRTypedefs.h"
#include "RPRXTypedefs.h"
#include "MaterialContext.h"
#include "ParameterArgs.h"

namespace RPRX
{
	DECLARE_DELEGATE_RetVal_FourParams(RPR::FResult, FUberMaterialParametersPropertyVisitor, const FRPRUberMaterialParameters&, UScriptStruct*, UProperty*, FMaterial&)

	class FMaterialCacheMaker
	{
	public:

		FMaterialCacheMaker(RPR::FMaterialContext& InMaterialContent, const URPRMaterial* InRPRMaterial, RPR::FImageManagerPtr InImageManager);

		bool	CacheUberMaterial(RPRX::FMaterial& OutMaterial);
		bool	UpdateUberMaterial(RPRX::FMaterial& InOutMaterial);

	private:

		RPR::FResult	BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, FMaterial& OutMaterial);
		RPR::FResult	ApplyUberMaterialParameter(const FRPRUberMaterialParameters& Parameters, UScriptStruct* ParametersStruct,
													UProperty* ParameterProperty, FMaterial& InOutMaterial);

		const FString&	GetMetaDataXmlParam(UProperty* Property) const;

	private:

		RPR::FMaterialContext&	MaterialContext;
		const URPRMaterial*		RPRMaterial;
		RPR::FImageManagerPtr	ImageManager;
	};

}

