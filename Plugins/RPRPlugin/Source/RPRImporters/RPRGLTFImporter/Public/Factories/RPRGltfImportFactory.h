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
    virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& InFilename, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled) override;

public:
    /** Load the glTF file to a GLTF::FData structure. */
    static bool LoadGLTFData(const FString& InFilename, TSharedPtr<GLTF::FData>& OutGLTFData);
    /** Load the glTF buffer files to a GLTF::FBufferCache cache object. */
    static bool LoadGLTFBuffers(const FString& InDirectory, TSharedPtr<GLTF::FData> InGLTFData, TSharedPtr<GLTF::FBufferCache>& OutGLTFBuffers);

private:
    UStaticMesh* CreateStaticMesh(int InMeshIndex);
    UMaterialInterface* CreateMaterial(int InMaterialIndex);
    UBlueprint* CreateBlueprint(const TMap<int, UStaticMesh*>& InStaticMeshes);

    /** Traverse node hierarchy of glTF scene and import nodes. */
    bool ImportNodes(const std::vector<int>& InNodeIndices, TMap<int, UStaticMesh*>& OutStaticMeshes);
    /** Import all static meshes and materials from a glTF node. */
    bool ImportNode(int InNodeIndex, TMap<int, UStaticMesh*>& OutStaticMeshes);

    /** Get triangle indices for the mesh primitive from the glTF buffer. */
    bool ExtractTriangleIndices(const GLTF::FMeshPrimitive& InPrim, TArray<uint32>& OutTriangleIndices);
    /** Get attribute data (i.e. POSITION, NORMALS, etc) from the glTF buffer. */
    template <typename T>
    bool ExtractAttributeData(const GLTF::FMeshPrimitive& InPrim, const std::string& AttributeName, TArray<T>& OutBufferSegment);
    /** Gets the size in bytes of the type the accessor points to. */
    int32 GetAccessorTypeSize(const GLTF::FAccessor& InAccessor);

    /** Creates a unique package name for an imported asset based on the glTF name. */
    FString MakeUniquePackageName(const std::string& InGLTFName);

private:

    /** The glTF context comprised of the imported glTF file structure, buffers, and selected import options. */
    FGLTFPtr GLTF;

    /** Index of the current scene being processed by the importer. */
    int CurrentSceneIndex;
};

template <typename T>
bool URPRGLTFImportFactory::ExtractAttributeData(const GLTF::FMeshPrimitive& InPrim, const std::string& AttributeName, TArray<T>& OutBufferSegment)
{
    auto It = InPrim.attributes.find(AttributeName);
    if (It != InPrim.attributes.end())
    {
        const GLTF::FAccessor& GLTFAccessor = GLTF->Data->accessors.at(It->second);
        const GLTF::FBufferView& GLTFBufferView = GLTF->Data->bufferViews.at(GLTFAccessor.bufferView);
        int32 MemLen = GLTFAccessor.count * GetAccessorTypeSize(GLTFAccessor);
        if (MemLen <= GLTFBufferView.byteLength)
        {
            GLTF->Buffers->Get(GLTFBufferView.buffer, GLTFBufferView.byteOffset + GLTFAccessor.byteOffset, MemLen, OutBufferSegment);
        }
    }
    return OutBufferSegment.Num() > 0;
}
