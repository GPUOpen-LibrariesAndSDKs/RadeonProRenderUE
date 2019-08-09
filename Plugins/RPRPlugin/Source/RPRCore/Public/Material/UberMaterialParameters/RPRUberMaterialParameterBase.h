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

#include "Containers/UnrealString.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Material/Tools/MaterialCacheMaker/ParameterArgs.h"
#include "RPRUberMaterialParameterBase.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FCanUseParameter, const struct FRPRUberMaterialParameterBase*)
DECLARE_DELEGATE_OneParam(FApplyParameter, RPRX::MaterialParameter::FArgs&)

UENUM()
enum class ESupportMode : uint8
{
	NotSupported,
	PreviewNotSupported,
	FullySupported
};

/*
* Base for all parameters used in the Uber material.
* It contains extra datas to import datas from the .rprmat
* and make links with the UE4 materials.
* If you want to add a new type for the Uber material,
* you should start from here.
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

public:

	FRPRUberMaterialParameterBase();
	FRPRUberMaterialParameterBase(
		const FString& InParamName,
		RPRX::FParameterType InRprxParamType,
		ESupportMode InPreviewSupportMode,
		FCanUseParameter InCanUseParameter = FCanUseParameter(),
        FApplyParameter InApplyParameterDelegate = FApplyParameter());

	virtual ~FRPRUberMaterialParameterBase() {}

	RPRX::FParameterType	GetRprxParamType() const;
	const FString&			GetParameterName() const;
	bool					CanUseParameter() const;
    bool                    HasCustomParameterApplier() const;
    void                    ApplyParameter(struct RPRX::MaterialParameter::FArgs& SetterParameters);
	bool					IsPreviewSupported() const;
	bool					IsPropertySupported() const;
	void					SetAdditionalInfoText(const FText& Text);

	virtual FString	GetPropertyName(UProperty* Property) const;
	virtual FString	GetPropertyTypeName(UProperty* Property) const;

public:

	UPROPERTY(VisibleAnywhere)
	ESupportMode SupportMode;

	UPROPERTY(VisibleAnywhere)
	FText		AdditionalInfoText;

private:

	UPROPERTY()
	FString		ParamName;

	UPROPERTY()
	uint32		RprxParamType;

	FCanUseParameter    CanUseParameterDelegate;
    FApplyParameter     ApplyParameterDelegate;
};
