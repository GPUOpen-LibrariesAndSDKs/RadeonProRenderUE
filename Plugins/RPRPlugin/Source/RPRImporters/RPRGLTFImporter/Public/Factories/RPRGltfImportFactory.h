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

	bool			ImportMaterials(TArray<URPRMaterial*>& OutMaterials);
	URPRMaterial*	ImportMaterial(RPRX::FMaterial NativeRPRMaterial);

	template<typename TSetter>
	void			SetMaterialParameter(RPRX::FMaterial NativeRPRMaterial, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter);

private:

	UObject*		Parent;
	EObjectFlags	Flags;
	FString			Filename;

};

template<typename TSetter>
void URPRGLTFImportFactory::SetMaterialParameter(RPRX::FMaterial NativeRPRMaterial, FRPRUberMaterialParameterBase* UberParameter, RPRX::FParameter Parameter)
{
	/*RPR::GLTF::Importer::IRPRMaterialParameterSetter* diffuseColorSetter = new RPR::GLTF::Importer::FRPRMaterialMapSetter();
	diffuseColorSetter->Set(NativeRPRMaterial, &UberParameter.Diffuse_Color, RPRX_UBER_MATERIAL_DIFFUSE_COLOR);

	delete diffuseColorSetter;*/
}
