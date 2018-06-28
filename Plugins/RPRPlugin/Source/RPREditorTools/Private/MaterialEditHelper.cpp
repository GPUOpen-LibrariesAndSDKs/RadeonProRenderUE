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
#include "MaterialEditHelper.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "RPRCpMaterial.h"
#include "MaterialEditor/DEditorParameterValue.h"

bool FMaterialEditHelper::OnEachMaterialParameter(UMaterialInterface* Material, FMaterialParameterBrowseDelegate Delegate, bool bUpdateMaterial)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	OnEachMaterialParameter(MaterialEditorInstance, Delegate);	

	if (bUpdateMaterial)
	{
		MaterialEditorInstance->CopyToSourceInstance();
	}

	return (true);
}

void FMaterialEditHelper::OnEachMaterialParameter(UMaterialEditorInstanceConstant* MaterialEditorInstance, FMaterialParameterBrowseDelegate Delegate)
{
	FEditorParameterGroup& parameterGroup = MaterialEditorInstance->ParameterGroups[0];
	TArray<UDEditorParameterValue*>& parameters = parameterGroup.Parameters;
	for (int32 i = 0; i < parameters.Num(); ++i)
	{
		Delegate.Execute(parameters[i]);
	}
}

bool FMaterialEditHelper::BindRouterAndExecute(UMaterialInterface* Material, const TMap<FName, FMaterialParameterBrowseDelegate>& Router, bool bUpdateMaterial)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* materialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	BindRouterAndExecute(materialEditorInstance, Router);

	if (bUpdateMaterial)
	{
		materialEditorInstance->CopyToSourceInstance();
	}

	return (true);
}

void FMaterialEditHelper::BindRouterAndExecute(UMaterialEditorInstanceConstant* MaterialEditorInstance, const TMap<FName, FMaterialParameterBrowseDelegate>& Router)
{
	OnEachMaterialParameter(MaterialEditorInstance, FMaterialParameterBrowseDelegate::CreateLambda([&Router](UDEditorParameterValue* ParameterValue)
	{
		if (const FMaterialParameterBrowseDelegate* func = Router.Find(FRPRCpMaterial::GetParameterName<UDEditorParameterValue>(*ParameterValue)))
		{
			func->Execute(ParameterValue);
		}
	}));
}

UMaterialEditorInstanceConstant* FMaterialEditHelper::CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant)
{
	UMaterialEditorInstanceConstant* materialEditorInstance =
		NewObject<UMaterialEditorInstanceConstant>((UObject*)GetTransientPackage(), NAME_None, RF_Transactional);

	materialEditorInstance->bUseOldStyleMICEditorGroups = false;

	if (MaterialInstanceConstant)
	{
		materialEditorInstance->SetSourceInstance(MaterialInstanceConstant);
	}
	return (materialEditorInstance);
}
