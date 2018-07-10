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

#include "CoreMinimal.h"
#include "Factories/IGLTFMaterialTranslationFactory.h"
#include "GLTFTypedefs.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "GLTFMaterialTranslationFactoryRPR.generated.h"

//~ Forward declares
struct FRPRUberMaterialParameters;
struct FRPRMaterialMap;

UCLASS(hidecategories = Object)
class UGLTFMaterialTranslationFactoryRPR : public UGLTFMaterialTranslationFactoryInterface
{
    GENERATED_UCLASS_BODY()

    //~ UFactory interface

    virtual FText GetDisplayName() const override;
    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, UObject* InContext, FFeedbackContext* InWarn);

public:

	//~ UGLTFMaterialTranslationFactoryInterface interface

    virtual bool InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial) override;

private:

    /** Translate the Uber Material Parameters into UMaterialExpressions. */
    void TranslateRPRUberMaterialParameters(const FRPRUberMaterialParameters& InUberMatParams);

    /** Create and set up an expression node for a RPRMaterialMap based on its mode. */
    UMaterialExpression* CreateMapExpression(const FRPRMaterialCoM& InMap);
	UMaterialExpression* CreateMapExpression(const FRPRMaterialCoMChannel1& InMap);

private:

    /** The RPR glTF material to be translated. */
    GLTF::FRPRMaterial GLTFRPRMaterial;

};
