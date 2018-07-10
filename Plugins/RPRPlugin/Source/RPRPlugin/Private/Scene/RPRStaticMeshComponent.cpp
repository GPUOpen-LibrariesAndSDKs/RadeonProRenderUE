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

#include "Scene/RPRStaticMeshComponent.h"

#include <map>
#include <set>
#include <memory>
#include <sstream>

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "Rendering/PositionVertexBuffer.h"
#include "StaticMeshResources.h"

#include "Helpers/RPRHelpers.h"

#include "RadeonProRenderInterchange.h"
#include "Scene/RPRInterchangeMaterial.h"
#include "RprSupport.h"

#include "RPRStats.h"
#include "Scene/RPRScene.h"
#include "Material/RPRMaterialBuilder.h"
#include "Async/Async.h"
#include "Helpers/RPRXHelpers.h"
#include "RPRCpStaticMesh.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateMeshes);

TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	URPRStaticMeshComponent::Cache;

URPRStaticMeshComponent::URPRStaticMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

TArray<FRPRCachedMesh>	URPRStaticMeshComponent::GetMeshInstances(UStaticMesh *mesh)
{
	if (!Cache.Contains(mesh))
		return TArray<FRPRCachedMesh>();
	TArray<FRPRCachedMesh>			instances;
	const TArray<FRPRCachedMesh>	&srcShapes = Cache[mesh];

	const uint32	shapeCount = srcShapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	newShape = nullptr;
		if (rprContextCreateInstance(Scene->m_RprContext, srcShapes[iShape].m_RprShape, &newShape) != RPR_SUCCESS)
		{
			for (int32 jShape = 0; jShape < instances.Num(); ++jShape)
				rprObjectDelete(instances[jShape].m_RprShape);
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *mesh->GetName());
			return TArray<FRPRCachedMesh>();
		}
		else
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance created from '%s' section %d"), *mesh->GetName(), iShape);
#endif
		}
		instances.Add(FRPRCachedMesh(newShape, srcShapes[iShape].m_UEMaterialIndex));
	}
	return instances;
}

void	URPRStaticMeshComponent::ClearCache(RPR::FScene scene)
{
	check(scene != nullptr);

	for (auto it = Cache.CreateIterator(); it; ++it)
	{
		TArray<FRPRCachedMesh>	&shapes = it->Value;

		const uint32 shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			check(shapes[iShape].m_RprShape != nullptr);
			RPR::SceneDetachShape(scene, shapes[iShape].m_RprShape);
			RPR::DeleteObject(shapes[iShape].m_RprShape);
		}
	}
	Cache.Empty();
}
#define RPR_UMS_INTEGRATION 1
#define RPR_UMS_DUMP_RPIF 0

rpr_material_node URPRStaticMeshComponent::CreateDefaultDummyShapeMaterial(uint32 iShape)
{
	rpr_shape					shape = m_Shapes[iShape].m_RprShape;
	rpr_material_node material = nullptr;

	// Default dummy material creation
	if (rprMaterialSystemCreateNode(Scene->m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
		return nullptr;
	}
	m_Shapes[iShape].m_RprMaterial = material;
	if (rprMaterialNodeSetInputF(m_Shapes[iShape].m_RprMaterial, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
		return nullptr;
	}
	if (rprShapeSetMaterial(shape, m_Shapes[iShape].m_RprMaterial) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
		return nullptr;
	}
	return material;
}

rpriExportRprMaterialResult URPRStaticMeshComponent::CreateXMLShapeMaterial(uint32 iShape, UMaterialInterface const * matInterface)
{
	const char* materialName = TCHAR_TO_ANSI(*matInterface->GetName());
	rpr_shape					shape = m_Shapes[iShape].m_RprShape;
	rpr_material_node material = nullptr;
	const UMaterial				*parentMaterial = matInterface != nullptr ? matInterface->GetMaterial() : nullptr;
	assert(parentMaterial != nullptr);

#if 0
	// We can only query the matInstance properties if this is actually a mat instance.
	const UMaterialInstance* matInstance = Cast<UMaterialInstance>(matInterface);
	if (matInterface && matInstance)
	{
		UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\t[SCALARS]:"));
		for (auto& param : matInstance->ScalarParameterValues)
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\tName=%s, Value=%f"), *param.ParameterName.GetPlainNameString(), param.ParameterValue);
		}

		UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\t[VECTORS]:"));
		for (auto& param : matInstance->VectorParameterValues)
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\tName=%s, Value=%f,%f,%f,%f"), *param.ParameterName.GetPlainNameString(),
				param.ParameterValue.R, param.ParameterValue.G, param.ParameterValue.B, param.ParameterValue.A);
		}

		UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\t[TEXTURES]:"));
		for (auto& param : matInstance->TextureParameterValues)
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\tName=%s"), *param.ParameterName.GetPlainNameString());
		}
	}
#endif

	// We have a match - go ahead and use the relevent material.
	bool isUberMaterial = false;
	void* xmlMaterial = Scene->m_materialLibrary.CreateMaterial(matInterface, Scene->m_RprContext, Scene->m_RprMaterialSystem, Scene->m_RprSupportCtx, isUberMaterial);
				
	// If we failed to create the xmlMaterial, go ahead with red default one and just log the error
	if (!xmlMaterial) {
		if (rprMaterialSystemCreateNode(Scene->m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &xmlMaterial) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create a default RPR material node"));
			return rpriExportRprMaterialResult{ 0, nullptr };
		}
					
		// We choose to ignore errors below - it won't happen.
		rprMaterialNodeSetInputF(xmlMaterial, "color", 1.0f, 0.5f, 0.5f, 1.0f);

	}

	// We must differentiate between an uber material handle and an rpr material node handle.
	if (isUberMaterial)
	{
		// Save the material.
		rprx_material uberMaterial = reinterpret_cast<rprx_material>(xmlMaterial);
		m_Shapes[iShape].m_RprxMaterial = uberMaterial;

		// Attach the material to the shape.
		if (rprxShapeAttachMaterial(Scene->m_RprSupportCtx, shape, uberMaterial) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign substituted XML RPR material to the RPR shape"));
			return rpriExportRprMaterialResult{ 0, nullptr };
		}

		// Commit the changes to the uber material.
		rpr_int result = rprxMaterialCommit(Scene->m_RprSupportCtx, uberMaterial);
		if (result != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("rprxMaterialCommit failed error %d"), result);
			return rpriExportRprMaterialResult{ 0, nullptr };
		}
		return rpriExportRprMaterialResult{ 1, uberMaterial };

	}
	else
	{
		// save the material
		m_Shapes[iShape].m_RprMaterial = xmlMaterial;
		
		// And use it!
		if (rprShapeSetMaterial(shape, xmlMaterial) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign substituted XML RPR material to the RPR shape"));
			return rpriExportRprMaterialResult{ 0, nullptr };
		}
	}

#if 0
	// More debug data
	{
		TArray<FName> names;
		TArray<FGuid> ids;
		parentMaterial->GetAllTextureParameterNames(names, ids);
		UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\t[GetAllTextureParameterNames]:"));
		for (auto& name : names)
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\tName=%s"), *name.GetPlainNameString());

			UTexture* value = nullptr;
			if (parentMaterial->GetTextureParameterValue(name, value))
			{
				UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\t\tGot value"));
			}
		}

		UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\t[GetUsedTextures]:"));
		TArray<UTexture*> textures;
		parentMaterial->GetUsedTextures(textures, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
		for (auto& texture : textures)
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\tName=%s"), *texture->GetName());
		}
	}
#endif
	return rpriExportRprMaterialResult{ 0, xmlMaterial };
}

#pragma optimize("",off)
bool	URPRStaticMeshComponent::BuildMaterials()
{
	const UStaticMeshComponent	*component = Cast<UStaticMeshComponent>(SrcComponent);
	check(component != nullptr);
	
	// Assign the materials on the instances: The cached geometry might be the same
	// But materials can be overriden on a component basis
	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape shape = m_Shapes[iShape].m_RprShape;
		rpr_int status = RPR_SUCCESS;

		// If we have a wrong index, it ll just return nullptr, and fallback to a dummy material
		UMaterialInterface	*matInterface = component->GetMaterial(m_Shapes[iShape].m_UEMaterialIndex);
		const UMaterial		*parentMaterial = matInterface != nullptr ? matInterface->GetMaterial() : nullptr;
		const char* materialName = matInterface != nullptr ? TCHAR_TO_ANSI(*matInterface->GetName()) : "";

		if (matInterface->IsA<URPRMaterial>())
		{
			BuildRPRMaterial(shape, Cast<URPRMaterial>(matInterface));
			continue;
		}

		// check the caches to see if we have already converted this material
		auto cacheIt = Scene->m_MaterialCache.find(materialName);

		if ( (cacheIt != Scene->m_MaterialCache.end()))
		{		
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("Found %s in Cache"), UTF8_TO_TCHAR(materialName));
#endif
			rpriExportRprMaterialResult cachedMaterial = cacheIt->second;
			RPR::FMaterialBuilder materialBuilder(Scene);
			materialBuilder.BindMaterialRawDatasToShape(cachedMaterial.type, cachedMaterial.data, shape);
			continue;
		}

		rpr_material_node	material = nullptr;
		if (parentMaterial == nullptr)
		{
			material = CreateDefaultDummyShapeMaterial(iShape);
			Scene->m_MaterialCache[materialName] = rpriExportRprMaterialResult{ 0, material };
			continue;
		}

		// Attempt to map UE material to one in the Radeon ProRender material library loaded from disk.
		if (Scene->m_materialLibrary.HasMaterialName(materialName))
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("Found %s"), UTF8_TO_TCHAR(materialName));
#endif
			rpriExportRprMaterialResult res = CreateXMLShapeMaterial(iShape, matInterface);
			if (res.data != nullptr) {
				if(Scene->m_MaterialCache.find(materialName) != Scene->m_MaterialCache.end())
				{
#ifdef RPR_VERBOSE
					UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("!!%s Already in material cache!!"), UTF8_TO_TCHAR(materialName));
#endif
				}
				Scene->m_MaterialCache[materialName] = res;

			} else
			{
				material = CreateDefaultDummyShapeMaterial(iShape);
				Scene->m_MaterialCache[materialName] = rpriExportRprMaterialResult{ 0, material };
			}
			continue;
		}


		// We block instance materials if their parent is not UMS Enabled.
		bool parentMaterialAllowed = true;
		const UMaterialInstance* matInstance = Cast<UMaterialInstance>(matInterface);
		const char* parentMaterialName = "";
		if (matInstance) {
			const UMaterialInterface *parentMatInstance = matInstance->Parent;
			parentMaterialName = TCHAR_TO_ANSI(*parentMatInstance->GetName());
			parentMaterialAllowed = Scene->m_UMSControl.IsMaterialUMSEnabled(parentMaterialName);
		}

#if WITH_EDITOR
		bool materialUMSEnabled = Scene->m_UMSControl.IsMaterialUMSEnabled(materialName);
		if (parentMaterialAllowed &&  materialUMSEnabled)
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("UMS Enabled for %s"), UTF8_TO_TCHAR(materialName));
#endif

			std::vector<rpri::generic::IMaterialGraph*> mgs;
			mgs.emplace_back(new UE4InterchangeMaterialGraph(matInterface));

			static char const UE4ImporterString[] = "UE4 Importer";
			rpriImportProperty importProps[] = {
				{ "Import", reinterpret_cast<uintptr_t>(UE4ImporterString) },
				{ "Num Materials", mgs.size() },
				{ "Material Import Array", reinterpret_cast<uintptr_t>(mgs.data()) }
			};
			uint32_t const numImportProps = sizeof(importProps) / sizeof(importProps[0]);

			rpriImportFromMemory(Scene->m_RpriContext, "Generic", numImportProps, importProps);

			std::vector<rpriExportRprMaterialResult> resultArray;
			resultArray.resize(mgs.size());

			rpriExportProperty exportProps[] = {
				{ "RPR Context", reinterpret_cast<uintptr_t>(&Scene->m_RprContext) },
				{ "RPR Material System", reinterpret_cast<uintptr_t>(&Scene->m_RprMaterialSystem) },
				{ "RPRX Context", reinterpret_cast<uintptr_t>(&Scene->m_RprSupportCtx) },
				{ "Num RPR Materials", static_cast<uintptr_t>(mgs.size()) },
				{ "RPR Material Result Array", reinterpret_cast<uintptr_t>(resultArray.data()) },
			};
			uint32_t const numExportProps = sizeof(exportProps) / sizeof(exportProps[0]);

			rpriExport(Scene->m_RpriContext, "RPR API Exporter", numExportProps, exportProps);

			Scene->m_MaterialCache[materialName] = resultArray[0];

			if (resultArray[0].type == 0)
			{
				rpr_material_node rprMatNode = reinterpret_cast<rpr_material_node>(resultArray[0].data);
				rprShapeSetMaterial(shape, rprMatNode);
			}
			else
			{
				rprx_material rprMatX = reinterpret_cast<rprx_material>(resultArray[0].data);
				if (rprxShapeAttachMaterial(Scene->m_RprSupportCtx, shape, rprMatX) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR X material to the RPR shape"));
				}
				if (rprxMaterialCommit(Scene->m_RprSupportCtx, rprMatX) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't commit RPR X material"));
				}
			}
		} else {
			if (materialUMSEnabled) {
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Fallback for material %s due to disallowed parent %s"), UTF8_TO_TCHAR(materialName), UTF8_TO_TCHAR(parentMaterialName));
			}
			else {
				if (matInstance) {
					UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Fallback for material %s INSTANCE OF %s"), UTF8_TO_TCHAR(materialName), UTF8_TO_TCHAR(parentMaterialName));
				}
				else {
					UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Fallback for material %s"), UTF8_TO_TCHAR(materialName));
				}
			}
			if (rprMaterialSystemCreateNode(Scene->m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
				return false;
			}
			m_Shapes[iShape].m_RprMaterial = material;
			Scene->m_MaterialCache[materialName] = { 0, material };

			if (rprMaterialNodeSetInputF(material, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS ||
				rprShapeSetMaterial(shape, material) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
				return false;
			}
		}
#endif // WITH_EDITOR
	}

	return true;
}

void URPRStaticMeshComponent::BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material)
{
	FObjectScopedLocked<FRPRXMaterialLibrary> rprMaterialLibrary = Scene->GetRPRMaterialLibrary();

	if (!rprMaterialLibrary->Contains(Material))
	{
		rprMaterialLibrary->CacheAndRegisterMaterial(Material);
	}
	else if (Material->IsMaterialDirty())
	{
		rprMaterialLibrary->RecacheMaterial(Material);
	}

	if (!m_OnMaterialChangedDelegateHandles.Contains(Material))
	{
		FDelegateHandle dlgHandle = Material->OnRPRMaterialChanged().AddUObject(this, &URPRStaticMeshComponent::OnUsedMaterialChanged);
		m_OnMaterialChangedDelegateHandles.Add(Material, dlgHandle);
	}

	ApplyRPRMaterialOnShape(Shape, Material);
}

bool URPRStaticMeshComponent::ApplyRPRMaterialOnShape(RPR::FShape& Shape, URPRMaterial* Material)
{
	auto rprMaterialLibrary = Scene->GetRPRMaterialLibrary();

	uint32 materialType;
	RPR::FMaterialRawDatas materialRawDatas;
	if (!rprMaterialLibrary->TryGetMaterialRawDatas(Material, materialType, materialRawDatas))
	{
		UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("Cannot get the material raw datas from the library."));
		return (false);
	}

	RPR::FMaterialBuilder materialBuilder(Scene);
	materialBuilder.BindMaterialRawDatasToShape(materialType, materialRawDatas, Shape);
	return (true);
}

#pragma optimize("",on)

static bool const FLIP_SURFACE_NORMALS = false;
static bool const FLIP_UV_Y = true;

bool	URPRStaticMeshComponent::Build()
{
	// Async load: SrcComponent can be nullptr if it was deleted from the scene
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	// TODO: Find a better way to cull unwanted geometry
	// The issue here is we collect ALL static mesh components,
	// including some geometry generated during play
	// like the camera or pawn etc
	//	if (Cast<AStaticMeshActor>(SrcComponent->GetOwner()) == nullptr)
	//		return false;
	static const FName	kStripTag = "RPR_Strip";
	const AActor		*actor = SrcComponent->GetOwner();
	if (Cast<ACameraActor>(actor) != nullptr ||
		Cast<APawn>(actor) != nullptr ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;


	// Note for runtime builds
	// All that data is probably stripped from runtime builds
	// So the solution would be to build all static meshes data before packaging
	// Placing that built data inside the static mesh UserData could be an option
	UStaticMeshComponent	*staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	check(staticMeshComponent != nullptr);
	UStaticMesh	*staticMesh = staticMeshComponent->GetStaticMesh();
	if (staticMesh == nullptr ||
		staticMesh->RenderData == nullptr ||
		staticMesh->RenderData->LODResources.Num() == 0)
		return false;
	TArray<FStaticMaterial>	const	   &staticMaterials = staticMesh->StaticMaterials;

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	// DEBUG CODE for checking winding order.
	enum class WindingOrder { CCW, CW };
	std::set<WindingOrder> windingOrders;

	TArray<FRPRCachedMesh>	shapes = GetMeshInstances(staticMesh);
	// TODO : Reset the if so the cache system works again
	if (true/*shapes.Num() == 0*/) // No mesh in cache ?
	{
		FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
		const FStaticMeshVertexBuffer	&srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodRes);
		const FPositionVertexBuffer		&srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodRes);
		const uint32					uvCount = srcVertices.GetNumTexCoords();

		// Guess: we need to create several RprObject
		// One for each section
		// To check with ProRender API
		uint32	sectionCount = lodRes.Sections.Num();
		for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
		{
			const FStaticMeshSection	&section = lodRes.Sections[iSection];
			const uint32				srcIndexStart = section.FirstIndex;
			const uint32				indexCount = section.NumTriangles * 3;

			TArray<FVector>		positions;
			TArray<FVector>		normals;
			TArray<FVector2D>	uvs;

			const uint32	vertexCount = (section.MaxVertexIndex - section.MinVertexIndex) + 1;
			if (vertexCount == 0)
				continue;

			// Lots of resizing ..
			positions.SetNum(vertexCount);
			normals.SetNum(vertexCount);
			if (uvCount > 0) // For now force set only one uv set
				uvs.SetNum(vertexCount * 1/*uvCount*/);

			TArray<int32>	indices;
			TArray<int32>	numFaceVertices;

			indices.SetNum(indexCount);
			numFaceVertices.SetNum(section.NumTriangles);

			const uint32	offset = section.MinVertexIndex;
			for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
			{
				const uint32	index = srcIndices[srcIndexStart + iIndex];
				const uint32	remappedIndex = index - offset;

				indices[iIndex] = remappedIndex;

				FVector	pos = srcPositions.VertexPosition(index) * 0.1f;
				FVector	normal = srcVertices.VertexTangentZ(index);
				positions[remappedIndex] = FVector(pos.X, pos.Z, pos.Y);
				if (FLIP_SURFACE_NORMALS)
				{
					normal = -normal;
				}
				normals[remappedIndex] = FVector(normal.X, normal.Z, normal.Y);

				if (uvCount > 0)
				{
					FVector2D uv = srcVertices.GetVertexUV(index, 0); // Right now only copy uv 0
					if(FLIP_UV_Y)
					{
						uv.Y = 1 - uv.Y;
					}
					uvs[remappedIndex] = uv;
				}
			}

			for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
				numFaceVertices[iTriangle] = 3;

			rpr_shape	shape = nullptr;

			if (rprContextCreateMesh(Scene->m_RprContext,
									 (rpr_float const *)positions.GetData(), positions.Num(), sizeof(float) * 3,
									 (rpr_float const *)normals.GetData(), normals.Num(), sizeof(float) * 3,
									 (rpr_float const *)uvs.GetData(), uvs.Num(), sizeof(float) * 2,
									 (rpr_int const *)indices.GetData(), sizeof(int32),
									 (rpr_int const *)indices.GetData(), sizeof(int32),
									 (rpr_int const *)indices.GetData(), sizeof(int32),
									 (rpr_int const *)numFaceVertices.GetData(), numFaceVertices.Num(),
									 &shape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"), *SrcComponent->GetName(), iSection, indices.Num(), positions.Num());
				return false;
			}

			// DEBUG CODE for checking winding orders.
			for (auto i = 0U; i < section.NumTriangles * 3; i += 3)
			{
				// Get the indices for the current triangle.
				auto i0 = indices[i];
				auto i1 = indices[i + 1];
				auto i2 = indices[i + 2];

				// Get the vertices for the current triangle.
				FVector v0 = positions.GetData()[i0];
				FVector v1 = positions.GetData()[i1];
				FVector v2 = positions.GetData()[i2];

				// Get the normals oc the current triangle.
				FVector n0 = normals.GetData()[i0];
				FVector n1 = normals.GetData()[i1];
				FVector n2 = normals.GetData()[i2];

#define USE_GEOMETRIC_NORMAL 0
#if USE_GEOMETRIC_NORMAL == 1
				FVector n = FVector::CrossProduct(v1 - v0, v2 - v0).GetSafeNormal();
#else
				FVector n = (n0 + n1 + n2) * 0.3333f;
#endif
				// Project vertices onto a 2D plane offset some distance along the triangle's surface normal.
				FVector planeNormal = n;
				FVector planeBase = (v0 + v1 + v2) * 0.5f + planeNormal;
				FVector p0 = FVector::PointPlaneProject(v0, planeBase, planeNormal);
				FVector p1 = FVector::PointPlaneProject(v0, planeBase, planeNormal);
				FVector p2 = FVector::PointPlaneProject(v0, planeBase, planeNormal);

				// Calculate the 2D determinant of the projected vertices onto the plane.
				float det = 0.5f * (p0.X * (p1.Y - p2.Y) + p1.X * (p2.Y - p0.Y) + p2.X * (p0.Y - p1.Y));
				if (det >= 0) windingOrders.emplace(WindingOrder::CCW);
				else windingOrders.emplace(WindingOrder::CW);
			}
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape created from '%s' section %d"), *staticMesh->GetName(), iSection);
#endif
			FRPRCachedMesh	newShape(shape, section.MaterialIndex);
			if (!Cache.Contains(staticMesh))
				Cache.Add(staticMesh);
			Cache[staticMesh].Add(newShape);

			// New shape in the cache ? Add it in the scene + make it invisible
			if (rprShapeSetVisibility(shape, false) != RPR_SUCCESS ||
				rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach Cached RPR shape to the RPR scene"));
				return false;
			}

			FRPRCachedMesh	newInstance(newShape.m_UEMaterialIndex);
			if (rprContextCreateInstance(Scene->m_RprContext, shape, &newInstance.m_RprShape) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *staticMesh->GetName());
				return false;
			}
			else
			{
#ifdef RPR_VERBOSE
				UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance created from '%s' section %d"), *staticMesh->GetName(), iSection);
#endif
			}
			m_Shapes.Add(newInstance);
		}

		if (windingOrders.size() > 1)
		{
			UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("\n\nMultiple winding orders found in shape!\n\n"));
		}
		else
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\nSingle winding order %s!\n\n"), ((*windingOrders.begin() == WindingOrder::CCW) ? TEXT("CCW") : TEXT("CW")));
#endif
		}
	}
	else
	{
		const uint32	shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
			m_Shapes.Add(shapes[iShape]);
	}

	static const FName		kPrimaryOnly("RPR_NoBlock");
	const bool				primaryOnly = staticMeshComponent->ComponentHasTag(kPrimaryOnly) || actor->ActorHasTag(kPrimaryOnly);
	const uint32			shapeCount = m_Shapes.Num();
	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld());
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		if (rprShapeSetTransform(shape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
			rprShapeSetVisibility(shape, staticMeshComponent->IsVisible()) != RPR_SUCCESS ||
			(primaryOnly && rprShapeSetVisibilityPrimaryOnly(shape, primaryOnly) != RPR_SUCCESS) ||
			rprShapeSetShadow(shape, staticMeshComponent->bCastStaticShadow) != RPR_SUCCESS ||
			rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			return false;
		}
	}
	return true;
}

bool	URPRStaticMeshComponent::PostBuild()
{
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	if (!BuildMaterials())
		return false;

	return Super::PostBuild();
}

bool URPRStaticMeshComponent::RPRThread_Update()
{
	const bool bNeedRebuild = AreMaterialsDirty();

	if (bNeedRebuild)
	{
		auto rprMaterialLibrary = Scene->GetRPRMaterialLibrary();
		FScopeLock scLock(&m_RefreshLock);

		URPRMaterial* material = nullptr;
		while (m_dirtyMaterialsQueue.Dequeue(material))
		{
			rprMaterialLibrary->RecacheMaterial(material);

			uint32 materialType;
			RPR::FMaterialRawDatas materialRawDatas;
			if (rprMaterialLibrary->TryGetMaterialRawDatas(material, materialType, materialRawDatas))
			{
				RPR::FMaterialBuilder materialBuilder(Scene);
				materialBuilder.CommitMaterial(materialType, reinterpret_cast<RPRX::FMaterial>(materialRawDatas));
			}
		}
	}

	return (bNeedRebuild | Super::RPRThread_Update());
}

void URPRStaticMeshComponent::OnUsedMaterialChanged(URPRMaterial* Material)
{
	auto rprMaterialLibrary = Scene->GetRPRMaterialLibrary();

	if (rprMaterialLibrary->Contains(Material))
	{
		m_dirtyMaterialsQueue.Enqueue(Material);
		MarkMaterialsAsDirty();
	}
	else
	{
		FDelegateHandle dlgHandle = m_OnMaterialChangedDelegateHandles.FindAndRemoveChecked(Material);
		Material->OnRPRMaterialChanged().Remove(dlgHandle);
	}
}

void URPRStaticMeshComponent::ClearMaterialChangedWatching()
{
	for (auto it(m_OnMaterialChangedDelegateHandles.CreateIterator()); it; ++it)
	{
		URPRMaterial* material = it.Key();
		material->OnRPRMaterialChanged().Remove(it.Value());
	}
	m_OnMaterialChangedDelegateHandles.Empty();
}

void URPRStaticMeshComponent::MarkMaterialsAsDirty()
{
	m_RebuildFlags |= PROPERTY_REBUILD_MATERIALS;
}

void	URPRStaticMeshComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateMeshes);

	Super::TickComponent(deltaTime, tickType, tickFunction);
}

bool	URPRStaticMeshComponent::RebuildTransforms()
{
	check(!IsInGameThread());

	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld());

	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		if (rprShapeSetTransform(m_Shapes[iShape].m_RprShape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't refresh RPR mesh transforms"));
			return false;
		}
	}
	return true;
}

bool URPRStaticMeshComponent::AreMaterialsDirty() const
{
	return ((m_RebuildFlags & PROPERTY_REBUILD_MATERIALS) != 0);
}

void	URPRStaticMeshComponent::ReleaseResources()
{
	if (m_Shapes.Num() > 0)
	{
		check(Scene != nullptr);
		uint32	shapeCount = m_Shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			if (m_Shapes[iShape].m_RprxMaterial != nullptr)
			{
				check(m_Shapes[iShape].m_RprShape != nullptr);
				RPRX::ShapeDetachMaterial(Scene->m_RprSupportCtx, m_Shapes[iShape].m_RprShape, m_Shapes[iShape].m_RprxMaterial);
			}

			if (m_Shapes[iShape].m_RprShape != nullptr)
			{
				RPR::SceneDetachShape(Scene->m_RprScene, m_Shapes[iShape].m_RprShape);
				RPR::DeleteObject(m_Shapes[iShape].m_RprShape);
			}
		}
		m_Shapes.Empty();
	}

	ClearMaterialChangedWatching();

	Super::ReleaseResources();
}
