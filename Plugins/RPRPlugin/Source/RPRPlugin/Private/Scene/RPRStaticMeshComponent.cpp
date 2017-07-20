// RPR COPYRIGHT

#include "RPRStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "RPRHelpers.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshResources.h"
#include "rprigenericapi.h"
#include <map>
#include <memory>
#include "RadeonProRenderInterchange.h"
#include <sstream>
#include "RPRInterchangeMaterial.h"
#include "RprSupport.h"

extern "C" void OutputDebugStringA(char const *);

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);
// chuck these up here for now. Move to own file asap
namespace
{
void    rpriLogger(char const * _log)
{
	OutputDebugStringA(_log);
}
}
TMap<UStaticMesh*, TArray<SRPRCachedMesh>>	URPRStaticMeshComponent::Cache;

URPRStaticMeshComponent::URPRStaticMeshComponent()
	:	m_RprMaterialSystem(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
}

TArray<SRPRCachedMesh>	URPRStaticMeshComponent::GetMeshInstances(UStaticMesh *mesh)
{
	if (!Cache.Contains(mesh))
		return TArray<SRPRCachedMesh>();
	TArray<SRPRCachedMesh>			instances;
	const TArray<SRPRCachedMesh>	&srcShapes = Cache[mesh];

	const uint32	shapeCount = srcShapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	newShape = NULL;
		if (rprContextCreateInstance(Scene->m_RprContext, srcShapes[iShape].m_RprShape, &newShape) != RPR_SUCCESS)
		{
			for (int32 jShape = 0; jShape < instances.Num(); ++jShape)
				rprObjectDelete(instances[jShape].m_RprShape);
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *SrcComponent->GetName());
			return TArray<SRPRCachedMesh>();
		}
		else
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance created from '%s' section %d"), *mesh->GetName(), iShape);
		}
		instances.Add(SRPRCachedMesh(newShape, srcShapes[iShape].m_UEMaterialIndex));
	}
	return instances;
}

void	URPRStaticMeshComponent::CleanCache()
{
	// Obviously this is context dependent
	// TODO : Put a safer cache system in place *or* ensure there can only be one context
	Cache.Empty();
}
#define RPR_UMS_INTEGRATION 1
#define RPR_UMS_DUMP_RPIF 0

bool	URPRStaticMeshComponent::BuildMaterials()
{
	const UStaticMeshComponent	*component = Cast<UStaticMeshComponent>(SrcComponent);
	check(component != NULL);

	// Assign the materials on the instances: The cached geometry might be the same
	// But materials can be overriden on a component basis
	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		// If we have a wrong index, it ll just return NULL, and fallback to a dummy material
		const UMaterialInterface	*matInterface = component->GetMaterial(m_Shapes[iShape].m_UEMaterialIndex);
		const UMaterial				*parentMaterial = matInterface != NULL ? matInterface->GetMaterial() : NULL;
		rpr_shape					shape = m_Shapes[iShape].m_RprShape;

#if 0
		// SAMPLE CODE
		// Get all texture is sampled by the material.
		if (matInterface != nullptr)
		{
			const FMaterialResource *mat_res = matInterface->GetMaterialResource(ERHIFeatureLevel::Type::SM5, EMaterialQualityLevel::Type::High);
			if (mat_res != nullptr)
			{
				const TArray < UTexture * > & textures_ = mat_res->GetReferencedTextures();

				for (size_t tex_ = 0; tex_ < textures_.Num(); tex_++)
				{                    
					UE_LOG(LogRPRStaticMeshComponent
						, Log
						, TEXT("Texture %s")
						, *textures_[tex_]->GetFName().ToString());
				}
			}            
		}
#endif

		rpr_material_node	material = NULL;
		if (parentMaterial == NULL)
		{
			// Default dummy material creation
			if (rprMaterialSystemCreateNode(m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
				return false;
			}
			m_Shapes[iShape].m_RprMaterial = material;
			if (rprMaterialNodeSetInputF(m_Shapes[iShape].m_RprMaterial, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
				return false;
			}
			if (rprShapeSetMaterial(shape, m_Shapes[iShape].m_RprMaterial) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
				return false;
			}
		}
		// Attempt to map UE material to one in the Radeon ProRender material library loaded from disk.
		const char* materialName = TCHAR_TO_ANSI(*matInterface->GetName());
		if (Scene->m_materialLibrary.HasMaterialName(materialName))
		{
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("Found %s"), UTF8_TO_TCHAR(materialName));

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

			// We have a match - go ahead and use the relevent material.
			rpr_material_node xmlMaterial = Scene->m_materialLibrary.CreateMaterial(matInterface, m_RprSupportCtx, m_RprMaterialSystem);

			// If we failed to create the xmlMaterial, go ahead with red default one and just log the error
			if (!xmlMaterial) {
				if (rprMaterialSystemCreateNode(m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &xmlMaterial) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create a default RPR material node"));
					return false;
				}

				// We choose to ignore errors below - it won't happen.
				rprMaterialNodeSetInputF(xmlMaterial, "color", 1.0f, 0.5f, 0.5f, 1.0f);

			}
			// save the material
			m_Shapes[iShape].m_RprMaterial = xmlMaterial;

			// And use it!
			if (rprShapeSetMaterial(shape, xmlMaterial) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign substituted XML RPR material to the RPR shape"));
				return false;
			}


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

				/*UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\t[Expressions]:"));
				for (auto& expression : parentMaterial->Expressions)
				{
					UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\tName=%s, %s"), expression->GetName(), *expression->GetCreationName().ToString());
				}*/
			}
			continue;
		}
#if RPR_UMS_INTEGRATION == 1
		// currently do 1 material at a time with no node sharing
		UE4InterchangeMaterialGraph *mg = nullptr;
		mg = new UE4InterchangeMaterialGraph(matInterface, parentMaterial);

		static char const UE4ImporterString[] = "UE4 Importer";
		rpri::generic::IMaterialGraph* first = mg;
		rpriImportProperty importProps[] = {
			{ "Import", reinterpret_cast<uintptr_t>(UE4ImporterString) },
			{ "Num Materials", 1 },
			{ "Material Import Array", reinterpret_cast<uintptr_t>(first) }
		};
		uint32_t const numImportProps = sizeof(importProps) / sizeof(importProps[0]);

		rpriContext ctx;
		rpriAllocateContext(&ctx);
		rpriErrorOptions(ctx, 5, false, false);
		rpriSetLoggers(ctx, rpriLogger, rpriLogger, rpriLogger);

		rpriImportFromMemory(ctx, "Generic", numImportProps, importProps);

#if RPR_UMS_DUMP_RPIF == 1
		static int testCounter = 0;
		std::stringstream ss;
		ss << testCounter++;
		std::string sss = "C:/Users/AMD/Source/Repos/AMD/RadeonProRenderUE/test" + ss.str();
		rpriExportProperty exportProps[] = {
			{ "Export Path", reinterpret_cast<uintptr_t>(sss.c_str()) },
		};
		uint32_t const numExportProps = sizeof(exportProps) / sizeof(exportProps[0]);

		rpriExport(ctx, "RPIF Exporter", numExportProps, exportProps);
#else
		std::vector<rpriExportRprMaterialResult> resultArray;
		resultArray.resize(1);
		rpriExportRprMaterialResult* firstResult = resultArray.data();

		rpriExportProperty exportProps[] = {
			{ "RPR Context", reinterpret_cast<uintptr_t>(&Scene->m_RprContext) },
			{ "RPR Material System", reinterpret_cast<uintptr_t>(&m_RprMaterialSystem) },
			{ "RPRX Context", reinterpret_cast<uintptr_t>(&m_RprSupportCtx) },
			{ "Num RPR Materials", static_cast<uintptr_t>(1) },
			{ "RPR Material Result Array", reinterpret_cast<uintptr_t>(firstResult) },
		};
		uint32_t const numExportProps = sizeof(exportProps) / sizeof(exportProps[0]);

		rpriExport(ctx, "RPR API Exporter", numExportProps, exportProps);

		if (firstResult->type == 0)
		{
			rpr_material_node rprMatNode = reinterpret_cast<rpr_material_node>(firstResult->data);
			rpr_int status = rprShapeSetMaterial(shape, rprMatNode);
		}
		else
		{
			rprx_material rprMatX = reinterpret_cast<rprx_material>(firstResult->data);
			rpr_int status = rprxShapeAttachMaterial(m_RprSupportCtx, shape, rprMatX);
			if (status != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR X material to the RPR shape"));
			}
			status = rprxMaterialCommit(m_RprSupportCtx, rprMatX);
			if (status != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't commit RPR X material"));
			}
		}

#endif
		rpriFreeContext(ctx);
#else
		if (rprMaterialSystemCreateNode(m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
			return false;
		}
		m_Shapes[iShape].m_RprMaterial = material;
		if (rprMaterialNodeSetInputF(material, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS ||
			rprShapeSetMaterial(shape, material) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
			return false;
		}
#endif
	}
	return true;
}

bool	URPRStaticMeshComponent::Build()
{
	// Async load: SrcComponent can be null if it was deleted from the scene
	if (Scene == NULL || SrcComponent == NULL)
		return false;

	// TODO: Find a better way to cull unwanted geometry
	// The issue here is we collect ALL static mesh components,
	// including some geometry generated during play
	// like the camera or pawn etc
	//	if (Cast<AStaticMeshActor>(SrcComponent->GetOwner()) == NULL)
	//		return false;
	static const FName	kStripTag = "RPR_Strip";
	const AActor		*actor = SrcComponent->GetOwner();
	if (Cast<ACameraActor>(actor) != NULL ||
		Cast<APawn>(actor) != NULL ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;

	// Not sure if material systems should be created on a per mesh level or per section
	if (rprContextCreateMaterialSystem(Scene->m_RprContext, 0, &m_RprMaterialSystem) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material system"));
		return false;
	}
	if (rprxCreateContext(m_RprMaterialSystem, RPRX_FLAGS_ENABLE_LOGGING, &m_RprSupportCtx) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material X system"));
		return false;
	}

	// Note for runtime builds
	// All that data is probably stripped from runtime builds
	// So the solution would be to build all static meshes data before packaging
	// Placing that built data inside the static mesh UserData could be an option
	UStaticMeshComponent	*staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	check(staticMeshComponent != NULL);
	UStaticMesh	*staticMesh = staticMeshComponent->GetStaticMesh();
	if (staticMesh == NULL ||
		staticMesh->RenderData == NULL ||
		staticMesh->RenderData->LODResources.Num() == 0)
		return false;
	TArray<FStaticMaterial>	const	   &staticMaterials = staticMesh->StaticMaterials;

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	TArray<SRPRCachedMesh>	shapes = GetMeshInstances(staticMesh);
	if (shapes.Num() == 0) // No mesh in cache ?
	{
		FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
		const FStaticMeshVertexBuffer	&srcVertices = lodRes.VertexBuffer;
		const FPositionVertexBuffer		&srcPositions = lodRes.PositionVertexBuffer;
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
				normals[remappedIndex] = FVector(normal.X, normal.Z, normal.Y);
				if (uvCount > 0)
					uvs[remappedIndex] = srcVertices.GetVertexUV(index, 0); // Right now only copy uv 0
			}

			for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
				numFaceVertices[iTriangle] = 3;

			rpr_shape	shape = NULL;

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



			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape created from '%s' section %d"), *staticMesh->GetName(), iSection);
			SRPRCachedMesh	newShape(shape, section.MaterialIndex);
			if (!Cache.Contains(staticMesh))
				Cache.Add(staticMesh);
			Cache[staticMesh].Add(newShape);
			m_Shapes.Add(newShape);
		}
	}
	else
	{
		const uint32	shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
			m_Shapes.Add(shapes[iShape]);
	}

	const uint32			shapeCount = m_Shapes.Num();
	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->ComponentToWorld);
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		if (rprShapeSetTransform(shape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
			rprShapeSetVisibility(shape, staticMeshComponent->IsVisible()) != RPR_SUCCESS ||
			rprShapeSetShadow(shape, staticMeshComponent->bCastStaticShadow) != RPR_SUCCESS ||
			rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			return false;
		}
	}
	return Super::Build();
}

bool	URPRStaticMeshComponent::PostBuild()
{
	if (!m_Built)
		return true; // We keep it anyway

	if (!BuildMaterials())
		return false;

	return Super::PostBuild();
}

bool	URPRStaticMeshComponent::RebuildTransforms()
{
	RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->ComponentToWorld);

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

void	URPRStaticMeshComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_Shapes.Num() > 0)
	{
		check(Scene != NULL);
		uint32	shapeCount = m_Shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			if (m_Shapes[iShape].m_RprShape != NULL)
			{
				rprSceneDetachShape(Scene->m_RprScene, m_Shapes[iShape].m_RprShape);
				rprObjectDelete(m_Shapes[iShape].m_RprShape);
			}
			if (m_Shapes[iShape].m_RprMaterial != NULL)
				rprObjectDelete(m_Shapes[iShape].m_RprMaterial);
		}
		m_Shapes.Empty();
	}
	if (m_RprMaterialSystem != NULL)
	{
		check(Scene != NULL);
		rprObjectDelete(m_RprMaterialSystem);
		m_RprMaterialSystem = NULL;
	}
}
