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

#include "SubImporters/StaticMeshesImporter.h"
#include "RPR_GLTF_Tools.h"
#include "Helpers/RPRHelpers.h"
#include "RPRMeshImporter.h"
#include "GTLFImportSettings.h"
#include "RPRGLTFImporterModule.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRShapeHelpers.h"
#include "AssetRegistryModule.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "Miscs/RPRMaterialNodeDumper.h"

bool RPR::GLTF::Import::FStaticMeshesImporters::ImportMeshes(
	const gltf::glTFAssetData& GLTFFileData, 
	RPR::GLTF::FMaterialResourcesPtr MaterialResources, 
	RPR::GLTF::FStaticMeshResourcesPtr StaticMeshesResources)
{
	RPR::FResult status;

	TArray<RPR::FShape> shapes;
	status = RPR::GLTF::Import::GetShapes(shapes);
	if (RPR::IsResultFailed(status))
	{
		return (false);
	}

	checkf(GLTFFileData.meshes.size() == shapes.Num(), TEXT("Count of mesh imported by gltf and RPR is different"));

	UGTLFImportSettings* gltfImportSettings = GetMutableDefault<UGTLFImportSettings>();
	RPR::FMeshImporter::FSettings importSettings;
	{
		importSettings.ScaleFactor = gltfImportSettings->ScaleFactor;
		importSettings.Rotation = gltfImportSettings->Rotation;
	}

	for (int32 i = 0; i < shapes.Num(); ++i)
	{
		FString meshName = FString(GLTFFileData.meshes[i].name.c_str());
		RPR::FShape shape = shapes[i];

		auto& resourceData = StaticMeshesResources->RegisterNewResource(i);
		resourceData.ResourceRPR = shape;

		UStaticMesh* staticMesh = RPR::FMeshImporter::ImportMesh(meshName, shape, importSettings);
		if (staticMesh != nullptr)
		{
			AttachMaterialsOnMesh(shape, staticMesh, MaterialResources);

			resourceData.ResourceUE4 = staticMesh;
			FAssetRegistryModule::AssetCreated(staticMesh);
		}
		else
		{
			UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Mesh import fail '%s'"), *meshName);
		}
	}

	return (true);
}

void RPR::GLTF::Import::FStaticMeshesImporters::AttachMaterialsOnMesh(RPR::FShape Shape, UStaticMesh* StaticMesh, RPR::GLTF::FMaterialResourcesPtr MaterialResources)
{
	RPR::FMaterialNode materialNode;
	RPR::FResult status = RPR::Shape::GetMaterial(Shape, materialNode);
	if (RPR::IsResultFailed(status) || materialNode == nullptr)
	{
		return;
	}

	RPRX::FContext rprxCtx = IRPRCore::GetResources()->GetRPRXSupportContext();
	bool bIsRPRXMaterial;
	status = RPRX::FMaterialHelpers::IsMaterialRPRX(rprxCtx, materialNode, bIsRPRXMaterial);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Cannot determine if the material is RPR or RPRX"));
		return;
	}

	if (!bIsRPRXMaterial)
	{
		return;
	}

	UE_LOG(LogRPRGLTFImporter, Log, TEXT("=== Dump node to attach ==="));
	RPR::RPRMaterial::DumpMaterialNode(IRPRCore::GetResources()->GetRPRContext(), materialNode, rprxCtx);

	RPRX::FMaterial rprxMaterial = (RPRX::FMaterial) materialNode;
	auto resourceData = MaterialResources->FindResourceByNativeMaterial(rprxMaterial);

	if (resourceData != nullptr)
	{
		URPRMaterial* ue4Material = resourceData->ResourceUE4;
		StaticMesh->SetMaterial(0, ue4Material);
	}
}
