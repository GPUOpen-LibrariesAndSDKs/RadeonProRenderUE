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
#include "Factories/Factory.h"
#include "Engine/Texture2D.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "GLTFTypedefs.h"
#include "IGLTFMaterialTranslationFactory.generated.h"

UCLASS(hidecategories = Object)
class UGLTFMaterialTranslationFactoryInterface : public UFactory
{
    GENERATED_UCLASS_BODY()

    //~ UFactory interface

    virtual bool DoesSupportClass(UClass* InClass) override;

public:

    /** Initializes the factory with glTF material structures. */
    virtual bool InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial) PURE_VIRTUAL(UGLTFMaterialTranslationFactoryInterface::InitFromGLTF, return false;);

protected:

    /** Create and set up a scalar constant expression. */
    UMaterialExpressionConstant* CreateScalarExpression(const float& InScalar);

    /** Create and set up a Vector4 constant expression. */
    UMaterialExpressionConstant4Vector* CreateVec4Expression(const FLinearColor& InVector4);

    /** Create and set up a TextureSample expression. */
    UMaterialExpressionTextureSample* CreateTextureSampleExpression(UTexture2D* InTexture);

    /** Create and set up a multiply expression node. */
    UMaterialExpressionMultiply* CreateMultiplyExpression(UMaterialExpression* InA, UMaterialExpression* InB);

protected:

    /** The glTF master object. */
    FGLTFPtr GLTF;

    /** The glTF material. */
    GLTF::FMaterial GLTFMaterial;

    /** The UE4 material to translate to. */
	UMaterialInterface* Material;

    /** The next Y-position to place a material expression node in the graph. */
    int32 MaterialNodeY;

    /** Increment MaterialNodeY. */
    int32 MaterialNodeStepY;
};
