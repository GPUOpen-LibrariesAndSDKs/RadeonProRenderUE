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

#include "Factories/Factory.h"
#include "Materials/MaterialInterface.h"
#include "RPRMaterialFactory.generated.h"

UCLASS()
class URPRMaterialFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:

	virtual bool		CanCreateNew() const;
	
	virtual UObject*	FactoryCreateFile(UClass* InClass, UObject* InParent, 
											FName InName, EObjectFlags Flags, 
											const FString& Filename, const TCHAR* Parms, 
											FFeedbackContext* Warn, bool& bOutOperationCanceled);

	virtual UObject*	FactoryCreateNew(UClass* InClass, UObject* InParent, 
											FName InName, EObjectFlags Flags, 
											UObject* Context, FFeedbackContext* Warn);

	virtual FText		GetDisplayName() const;
	virtual uint32		GetMenuCategories() const;
	virtual FText		GetToolTip() const;
	virtual FString		GetDefaultNewAssetName() const;
	virtual FName		GetNewAssetThumbnailOverride() const;

	static bool			LoadRPRMaterialFromXmlFile(class URPRMaterial* RPRMaterial, const FString& Filename);

private:

	UMaterialInterface*	TryLoadUberMaterial(FFeedbackContext* Warn);
	static void			LoadRPRMaterialParameters(class URPRMaterial* RPRMaterial, class FRPRMaterialXmlGraph& MaterialGraph, const FString& Filename);
	static void			CopyRPRMaterialParameterToMaterialInstance(class URPRMaterial* RPRMaterial);

	static class URPRMaterialEditorInstanceConstant*	CreateMaterialEditorInstanceConstantFrom(class URPRMaterial* RPRMaterial);
};
