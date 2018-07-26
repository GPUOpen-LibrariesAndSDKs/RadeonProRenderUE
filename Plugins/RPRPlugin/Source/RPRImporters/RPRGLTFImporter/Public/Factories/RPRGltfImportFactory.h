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
#include "GLTFTypedefs.h"
#include "Assets/RPRMaterial.h"
#include "Typedefs/RPRXTypedefs.h"
#include "ImageManager/RPRImageManager.h"
#include "gltf/gltf2.h"
#include "Resources/MaterialResources.h"
#include "Resources/ImageResources.h"
#include "RPRGLTFImportFactory.generated.h"

//~ Forward declares
class UStaticMesh;
class UMaterialInterface;
class UBlueprint;

UCLASS(hidecategories = Object)
class URPRGLTFImportFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    //~ UFactory interface

    virtual FText GetDisplayName() const override;
    virtual bool DoesSupportClass(UClass* InClass) override;
    virtual bool FactoryCanImport(const FString& InFilename) override;
    virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFilename, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled) override;

private:

	bool			ImportImages(const gltf::glTFAssetData& GLTFFileData, RPR::GLTF::FImageResourcesPtr ImageResources);
	bool			ImportMaterials(const gltf::glTFAssetData& GLTFFileData, RPR::GLTF::FImageResourcesPtr ImageResources, RPR::GLTF::FMaterialResourcesPtr MaterialResources);
	URPRMaterial*	ImportMaterial(const FString& MaterialName, RPR::GLTF::FImageResourcesPtr ImageResources, RPRX::FMaterial NativeRPRMaterial);
	bool			ImportMeshes(const gltf::glTFAssetData& GLTFFileData, RPR::GLTF::FMaterialResourcesPtr MaterialResources, TArray<UStaticMesh*>& OutStaticMeshes);
	UStaticMesh*	ImportMesh(const FString& MeshName, RPR::FShape Shape);
	void			AttachMaterialsOnMesh(RPR::FShape Shape, UStaticMesh* StaticMesh, RPR::GLTF::FMaterialResourcesPtr MaterialResources);

	void			GetImagePathsFromGLTF(const gltf::glTFAssetData& GLTFFileData, TArray<FString>& OutImagePaths);
	void			LoadTextures(const TArray<FString>& ImagePaths, RPR::GLTF::FImageResourcesPtr ImageResources);

	template<typename TSetter>
	void			SetMaterialParameter(RPRX::FMaterial NativeRPRMaterial, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter);

	URPRMaterial*	CreateNewMaterial(const FString& MaterialName) const;

private:

	UObject*		Parent;
	EObjectFlags	Flags;
	FString			Filename;
};