// RPR COPYRIGHT

#include "RPRStaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

URPRStaticMeshComponent::URPRStaticMeshComponent()
:	m_RprMaterialSystem(NULL)
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool	URPRStaticMeshComponent::Build()
{
	if (Scene == NULL || SrcComponent == NULL)
		return false;

	// TODO: Find a better way to cull unwanted geometry
	// The issue here is we collect ALL static mesh components,
	// including some geometry generated during play
	// like the camera or pawn etc
	if (Cast<AStaticMeshActor>(SrcComponent->GetOwner()) == NULL)
		return false;

	// Not sure if material systems should be created on a per mesh level or per section
	if (rprContextCreateMaterialSystem(Scene->m_RprContext, 0, &m_RprMaterialSystem) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material system"));
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

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
	const uint32					totalIndexCount = uint32(srcIndices.Num());
	const uint32					totalVertexCount = lodRes.PositionVertexBuffer.GetNumVertices();

	const FStaticMeshVertexBuffer	&srcVertices = lodRes.VertexBuffer;
	const uint32					uvCount = srcVertices.GetNumTexCoords();

	const FPositionVertexBuffer		&srcPositions = lodRes.PositionVertexBuffer;

	TArray<FVector>		positions;
	TArray<FVector>		normals;
	TArray<FVector2D>	uvs;
	TArray<int32>		indices;
	TArray<int32>		numFaceVertices;

	positions.SetNum(totalVertexCount);
	normals.SetNum(totalVertexCount);
	if (uvCount > 0) // For now force set only one uv set
		uvs.SetNum(totalVertexCount * 1/*uvCount*/);
	indices.SetNum(totalIndexCount);
	numFaceVertices.SetNum(totalIndexCount);

	// Guess: we need to create several RprObject
	// One for each section
	// To check with ProRender API
	uint32	sectionCount = lodRes.Sections.Num();
	for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
	{
		const FStaticMeshSection	&section = lodRes.Sections[iSection];
		const uint32				srcIndexStart = section.FirstIndex;
		const uint32				indexCount = section.NumTriangles * 3;

		for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
		{
			const uint32	srcVertex = srcIndices[srcIndexStart + iIndex];

			indices[srcIndexStart + iIndex] = srcVertex;
			numFaceVertices[srcIndexStart + iIndex] = 3;
			FVector	pos = srcPositions.VertexPosition(srcVertex) * 0.1f;
			positions[srcVertex] = FVector(pos.X, pos.Z, pos.Y);
			FVector	normal = srcVertices.VertexTangentZ(srcVertex);
			normals[srcVertex] = FVector(normal.X, normal.Z, normal.Y);
			if (uvCount > 0)
				uvs[srcVertex] = srcVertices.GetVertexUV(srcVertex, 0); // Right now only copy uv 0
		}

		rpr_shape	shape = NULL;
		if (rprContextCreateMesh(Scene->m_RprContext,
			(rpr_float const *)positions.GetData(), positions.Num(), sizeof(float) * 3,
			(rpr_float const *)normals.GetData(), normals.Num(), sizeof(float) * 3,
			(rpr_float const *)uvs.GetData(), uvs.Num(), sizeof(float) * 2,
			(rpr_int const *)indices.GetData(), sizeof(int32),
			(rpr_int const *)indices.GetData(), sizeof(int32),
			(rpr_int const *)indices.GetData(), sizeof(int32),
			(rpr_int const *)numFaceVertices.GetData(), indices.Num() / 3,
			&shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh"));
			return false;
		}
		rpr_material_node	material = NULL;
		if (rprMaterialSystemCreateNode(m_RprMaterialSystem, RPR_MATERIAL_NODE_DIFFUSE, &material) != RPR_SUCCESS)
		{
			rprObjectDelete(shape);
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR material node"));
			return false;
		}
		if (rprMaterialNodeSetInputF(material, "color", 0.5f, 0.5f, 0.5f, 1.0f) != RPR_SUCCESS ||
			rprShapeSetMaterial(shape, material) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
			rprObjectDelete(shape);
			rprObjectDelete(material);
			return false;
		}
		FVector						actorLocation = SrcComponent->ComponentToWorld.GetLocation() * 0.1f;
		FVector						actorScale = SrcComponent->ComponentToWorld.GetScale3D();
		RadeonProRender::float3		location(actorLocation.X, actorLocation.Z, actorLocation.Y);
		RadeonProRender::float3		scale(actorScale.X, actorScale.Z, actorScale.Y);
		RadeonProRender::matrix		matrix = RadeonProRender::translation(location);// + RadeonProRender::scale(scale);
		if (rprShapeSetTransform(shape, RPR_TRUE, &matrix.m00) != RPR_SUCCESS ||
			rprSceneAttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			rprObjectDelete(shape);
			return false;
		}
		UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape created from '%s'"), *SrcComponent->GetName());
		m_Shapes.Add(SRPRShape(shape, material));
	}
	return true;
}

void	URPRStaticMeshComponent::BeginDestroy()
{
	Super::BeginDestroy();
	if (m_RprMaterialSystem != NULL)
	{
		rprObjectDelete(m_RprMaterialSystem);
		m_RprMaterialSystem = NULL;
	}
	// TODO: Check if we need to call rprSceneDetachShape or rprObjectDelete does this thing for us
	uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rprObjectDelete(m_Shapes[iShape].m_RprShape);
		rprObjectDelete(m_Shapes[iShape].m_RprMaterial);
	}
	m_Shapes.Empty();
}
