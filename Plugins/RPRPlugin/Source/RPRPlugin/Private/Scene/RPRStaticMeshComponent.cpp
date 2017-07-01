// RPR COPYRIGHT

#include "RPRStaticMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

URPRStaticMeshComponent::URPRStaticMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool	URPRStaticMeshComponent::Build()
{
	if (Scene == NULL ||
		SrcComponent == NULL)
		return false;

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
			positions[srcVertex] = srcPositions.VertexPosition(srcVertex);
			normals[srcVertex] = srcVertices.VertexTangentZ(srcVertex);
			if (uvCount > 0)
				uvs[srcVertex] = srcVertices.GetVertexUV(srcVertex, 0); // Right now only copy uv 0
		}

		rpr_shape	newShape = NULL;
		if (rprContextCreateMesh(Scene->m_RprContext,
			(rpr_float const *)positions.GetData(), positions.Num(), sizeof(float) * 3,
			(rpr_float const *)normals.GetData(), normals.Num(), sizeof(float) * 3,
			(rpr_float const *)uvs.GetData(), uvs.Num(), sizeof(float) * 2,
			(rpr_int const *)indices.GetData(), sizeof(int32),
			(rpr_int const *)indices.GetData(), sizeof(int32),
			(rpr_int const *)indices.GetData(), sizeof(int32),
			(rpr_int const *)numFaceVertices.GetData(), indices.Num() / 3,
			&newShape) != RPR_SUCCESS)
			return false;
		m_RprObjects.Add(newShape);
	}
	return true;
}

void	URPRStaticMeshComponent::BeginDestroy()
{
	Super::BeginDestroy();
	uint32	objectCount = m_RprObjects.Num();
	for (uint32 iObj = 0; iObj < objectCount; ++iObj)
	{
		if (m_RprObjects[iObj] != NULL)
			rprObjectDelete(m_RprObjects[iObj]);
	}
	m_RprObjects.Empty();
}
