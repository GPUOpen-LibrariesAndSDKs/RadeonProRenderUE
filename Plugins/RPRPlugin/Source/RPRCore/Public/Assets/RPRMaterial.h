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

#include "Materials/MaterialInstanceConstant.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/TriPlanarSettings.h"
#include "RPRMaterial.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FRPRMaterialChanged, class URPRMaterial*)

/*
* Asset representing a RPR Uber material
*/
UCLASS(BlueprintType)
class RPRCORE_API URPRMaterial : public UMaterialInstanceConstant
{
	GENERATED_BODY()

	friend class FRPRXMaterialLibrary;

public:

	URPRMaterial();

	void					MarkMaterialDirty();
	bool					IsMaterialDirty() const;
	FRPRMaterialChanged&	OnRPRMaterialChanged() { return OnRPRMaterialChangedEvent; }

#if WITH_EDITOR
	virtual void	PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	void	ResetMaterialDirtyFlag();

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	FRPRUberMaterialParameters	MaterialParameters;

private:

	FRPRMaterialChanged OnRPRMaterialChangedEvent;
	bool				bShouldCacheBeRebuild;

};
