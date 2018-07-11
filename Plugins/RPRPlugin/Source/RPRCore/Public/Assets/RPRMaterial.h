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
