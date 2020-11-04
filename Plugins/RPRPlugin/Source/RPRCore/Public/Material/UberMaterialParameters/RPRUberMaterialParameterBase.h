/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "Containers/UnrealString.h"
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
		unsigned int InRprxParamType,
		ESupportMode InPreviewSupportMode,
		FCanUseParameter InCanUseParameter = FCanUseParameter(),
        FApplyParameter InApplyParameterDelegate = FApplyParameter());

	virtual ~FRPRUberMaterialParameterBase() {}

	unsigned int	        GetRprxParamType() const;
	const FString&			GetParameterName() const;
	bool					CanUseParameter() const;
    bool                    HasCustomParameterApplier() const;
    void                    ApplyParameter(struct RPRX::MaterialParameter::FArgs& SetterParameters);
	bool					IsPreviewSupported() const;
	bool					IsPropertySupported() const;
	void					SetAdditionalInfoText(const FText& Text);

	virtual FString	GetPropertyName(UProperty* Property) const;
	virtual FString	GetPropertyTypeName(UProperty* Property) const;

private:
	UPROPERTY()
	FString		ParamName;

	UPROPERTY()
	uint32		RprxParamType;

	UPROPERTY(VisibleAnywhere, Category = Material)
	ESupportMode SupportMode;

	UPROPERTY(VisibleAnywhere, Category = Material)
	FText		AdditionalInfoText;

	FCanUseParameter    CanUseParameterDelegate;
    FApplyParameter     ApplyParameterDelegate;
};
