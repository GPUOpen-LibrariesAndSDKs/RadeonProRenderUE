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

#include "UObject/Object.h"
#include "GLTFSettings.generated.h"

UCLASS(MinimalAPI, Config=EditorPerProjectUserSettings)
class UGLTFSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	/** The factor by which to scale meshes being imported. */
	UPROPERTY(EditAnywhere, Category=Import)
	float ScaleFactor;
	
	/** If true, import materials as double-sided. */
	UPROPERTY(EditAnywhere, Category=Import)
	bool TwoSidedMaterials;
	
	/** If true, the importer will only import the default scene. */
	UPROPERTY(EditAnywhere, Category=Import)
	bool DefaultSceneOnly;
	
	/** If true, the importer will compose the glTF scenes into a Blueprint hierarchy. */
	UPROPERTY(EditAnywhere, Category=Import)
	bool UseBlueprint;
	
	/** The exporter used to create the glTF file. */
    UPROPERTY(VisibleAnywhere, Category=Info, meta=(DisplayName="Created With"))
    FString FileGenerator;
	
	/** The path of the glTF file being imported. */
	UPROPERTY()
	FString FilePathInOS;
	
	/** The path of the resulting asset to be imported in the content browser. */
	UPROPERTY()
    FString FilePathInEngine;
};
