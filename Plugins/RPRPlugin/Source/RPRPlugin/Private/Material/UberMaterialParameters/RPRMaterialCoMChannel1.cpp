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
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Helpers/RPRConstAway.h"

FRPRMaterialCoMChannel1::FRPRMaterialCoMChannel1(
	const FString& InXmlParamName, 
	uint32 InRprxParamID, 
	ESupportMode InPreviewSupportMode, 
	float InConstantValue, 
	ERPRMCoMapC1InterpretationMode InMode, 
	FCanUseParameter InCanUseParameter)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter)
	, Constant(InConstantValue)
	, Mode(ERPRMaterialMapMode::Constant)
	, RPRInterpretationMode(InMode)
{
}

#if WITH_EDITOR

FNumericRestriction<float>& FRPRMaterialCoMChannel1::GetConstantRestriction()
{
	const FRPRMaterialCoMChannel1* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetConstantRestriction()));
}

const FNumericRestriction<float>& FRPRMaterialCoMChannel1::GetConstantRestriction() const
{
	return (ConstantRestriction);
}

#endif
