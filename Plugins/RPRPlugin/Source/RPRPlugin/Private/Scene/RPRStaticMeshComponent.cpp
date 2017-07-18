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
#define RPR_UMS_INTEGRATION 0
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
		const UMaterialInterface	*matInstance = component->GetMaterial(m_Shapes[iShape].m_UEMaterialIndex);
		const UMaterial				*parentMaterial = matInstance != NULL ? matInstance->GetMaterial() : NULL;
		rpr_shape					shape = m_Shapes[iShape].m_RprShape;

		rpr_material_node	material = NULL;
#if RPR_UMS_INTEGRATION == 1
		if (parentMaterial == NULL)
#else
		if(true)
#endif
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
				rprObjectDelete(material);
				return false;
			}
			if (rprShapeSetMaterial(shape, m_Shapes[iShape].m_RprMaterial) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
				rprObjectDelete(material);
				return false;
			}
		}
		else
		{
			// Here code for Sean, namechecks
			// Uncomment all the necessary code below and replace with your tests
			if (parentMaterial->GetName() == "SomeMaterial1")
			{
				// ...
			}
			else if (parentMaterial->GetName() == "Rubyredsomething")
			{
				// ...
			}

#if RPR_UMS_INTEGRATION == 1
			// currently do 1 material at a time with no node sharing
			UE4InterchangeMaterialGraph *mg = nullptr;
			mg = new UE4InterchangeMaterialGraph(parentMaterial);

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

			if(firstResult->type == 0)
			{
				rpr_material_node rprMatNode = reinterpret_cast<rpr_material_node>(firstResult->data);
				rprShapeSetMaterial(shape, rprMatNode);
			} else
			{
				rprx_material rprMatX = reinterpret_cast<rprx_material>(firstResult->data);
				rprxMaterialCommit(m_RprSupportCtx, rprMatX);
				rprxShapeAttachMaterial(m_RprSupportCtx, shape, rprMatX);
			}

#endif
			rpriFreeContext(ctx);
#else
			if (rprMaterialSystemCreateNode(m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
				rprObjectDelete(material);
				return false;
			}
			if (rprMaterialNodeSetInputF(material, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS ||
				rprShapeSetMaterial(shape, material) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
				rprObjectDelete(material);
				return false;
			}
#endif
		}
		RadeonProRender::matrix	matrix = BuildMatrixWithScale(SrcComponent->ComponentToWorld);
		if (rprShapeSetTransform(shape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
			rprShapeSetVisibility(shape, SrcComponent->IsVisible()) != RPR_SUCCESS ||
			rprShapeSetShadow(shape, component->bCastStaticShadow) != RPR_SUCCESS ||
			rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			rprObjectDelete(material);
			return false;
		}
		m_Shapes[iShape].m_RprMaterial = material;
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
	if (Cast<ACameraActor>(SrcComponent->GetOwner()) != NULL ||
		Cast<APawn>(SrcComponent->GetOwner()) != NULL)
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
