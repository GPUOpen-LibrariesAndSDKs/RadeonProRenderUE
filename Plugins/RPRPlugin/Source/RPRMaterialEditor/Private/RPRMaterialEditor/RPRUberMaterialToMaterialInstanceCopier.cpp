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
#include "RPRUberMaterialToMaterialInstanceCopier.h"
#include "RPRUberMaterialParameters.h"
#include "Map.h"
#include "IRPRMatParamCopier.h"
#include "RPRMatParamCopier_MaterialCoM.h"
#include "RPRMatParamCopier_MaterialCoMChannel1.h"
#include "RPRMaterialBool.h"
#include "RPRMaterialEnum.h"
#include "RPRMatParamCopier_Enum.h"
#include "RPRMatParamCopier_Bool.h"
#include "RPRMaterialCoMChannel1.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRUberMaterialToMaterialInstanceCopier, Log, All)

#define GET_CLASS_NAME_CHECKED(ClassName) \
	((void)sizeof(ClassName), TEXT(#ClassName))

void FRPRUberMaterialToMaterialInstanceCopier::CopyParameters(const FRPRUberMaterialParameters& RPRUberMaterialParameters, 
																UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	DECLARE_DELEGATE_ThreeParams(FApplyMat, const FRPRUberMaterialParameters&, UStructProperty*, UMaterialEditorInstanceConstant*);

	static TMap<FName, IRPRMatParamCopierPtr> applyRouter;
	
	if (applyRouter.Num() == 0)
	{
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialCoM), MakeShareable(new FRPRMatParamCopier_MaterialCoM));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialCoMChannel1), MakeShareable(new FRPRMatParamCopier_MaterialCoMChannel1));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialMap), MakeShareable(new FRPRMatParamCopier_MaterialMap));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialBool), MakeShareable(new FRPRMatParamCopier_Bool));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialEnum), MakeShareable(new FRPRMatParamCopier_Enum));
	}

	UStruct* uberMaterialParametersStruct = FRPRUberMaterialParameters::StaticStruct();
	UStructProperty* property = FindNextStructProperty(uberMaterialParametersStruct->PropertyLink);
	while (property != nullptr)
	{
		const FName propertyTypeName = *property->Struct->GetStructCPPName();
		IRPRMatParamCopierPtr* applyFunc = applyRouter.Find(propertyTypeName);
		if (applyFunc != nullptr)
		{
			(*applyFunc)->Apply(RPRUberMaterialParameters, property, RPRMaterialEditorInstance);
		}
		else
		{
			UE_LOG(LogRPRUberMaterialToMaterialInstanceCopier, Warning, TEXT("Class '%s' not supported!"), *propertyTypeName.ToString());
		}

		property = FindNextStructProperty(property->NextRef);
	}
}

UStructProperty* FRPRUberMaterialToMaterialInstanceCopier::FindNextStructProperty(UProperty* Property)
{
	while (Property != nullptr)
	{
		UStructProperty* nextStructProperty = Cast<UStructProperty>(Property);
		if (nextStructProperty != nullptr)
		{
			return (nextStructProperty);
		}

		Property = Property->NextRef;
	}

	return (nullptr);
}

#undef GET_CLASS_NAME_CHECKED
