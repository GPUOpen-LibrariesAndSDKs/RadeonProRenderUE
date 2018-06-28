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
#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "RPRCpMaterial.h"

const FName FTriPlanarMaterialEnabler::MaterialParameterName_UseTriPlanar(TEXT("UseTriPlanar"));
const FName FTriPlanarMaterialEnabler::MaterialParameterName_TextureAngle(TEXT("TriPlanar_TextureAngle"));
const FName FTriPlanarMaterialEnabler::MaterialParameterName_TextureScale(TEXT("TriPlanar_TextureScale"));

bool FTriPlanarMaterialEnabler::Enable(UMaterialInterface* Material, bool bEnable)
{
	check(Material);

	UMaterialInstanceConstant* UnrealMaterialConstant = Cast<UMaterialInstanceConstant>(Material);
	if (UnrealMaterialConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* materialEditorInstance = CreateMaterialEditorInstanceConstant(UnrealMaterialConstant);
	if (materialEditorInstance == nullptr)
	{
		return (false);
	}

	bool bOk = Enable(materialEditorInstance, bEnable);
	if (bOk)
	{
		Material->PostEditChange();
	}

	return (bOk);
}

bool FTriPlanarMaterialEnabler::Enable(UMaterialEditorInstanceConstant* MaterialEditorInstance, bool bEnable)
{
	check(MaterialEditorInstance);

	FEditorParameterGroup& parameterGroup = MaterialEditorInstance->ParameterGroups[0];
	TArray<UDEditorParameterValue*> parameterValues = parameterGroup.Parameters;
	for (int32 parameterIndex = 0; parameterIndex < parameterValues.Num(); ++parameterIndex)
	{
		UDEditorParameterValue* parameterValue = parameterValues[parameterIndex];
		if (FRPRCpMaterial::GetParameterName(*parameterValue) == MaterialParameterName_UseTriPlanar)
		{
			UDEditorStaticSwitchParameterValue* staticSwitch = Cast<UDEditorStaticSwitchParameterValue>(parameterValue);
			if (staticSwitch)
			{
				staticSwitch->ParameterValue = bEnable;
				staticSwitch->bOverride = true;

				MaterialEditorInstance->CopyToSourceInstance();
				return (true);
			}
		}
	}

	return (false);
}

bool FTriPlanarMaterialEnabler::Enable(UStaticMesh* StaticMesh, int32 SectionIndex, bool bEnable)
{
	return (Enable(StaticMesh->GetMaterial(SectionIndex), bEnable));
}

UMaterialEditorInstanceConstant* FTriPlanarMaterialEnabler::CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant)
{
	UMaterialEditorInstanceConstant* MaterialEditorInstance =
		NewObject<UMaterialEditorInstanceConstant>((UObject*)GetTransientPackage(), NAME_None, RF_Transactional);

	MaterialEditorInstance->bUseOldStyleMICEditorGroups = false;
	MaterialEditorInstance->SetSourceInstance(MaterialInstanceConstant);
	return (MaterialEditorInstance);
}
