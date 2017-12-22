#include "UVProjectionPlanarAlgo.h"
#include "UVUtility.h"
#include "Engine/StaticMesh.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshVertexBuffer.h"
#include "TransformablePlane.h"

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	Plane = FTransformablePlane(FPlane(FVector::ZeroVector, FVector::RightVector), FVector::ZeroVector, FVector::UpVector);

	const FPositionVertexBuffer* vertexBuffer = GetStaticMeshPositionVertexBuffer();
	if (vertexBuffer == nullptr)
	{
		StopAlgorithmAndRaiseCompletion(false);
		return;
	}

	PrepareUVs(vertexBuffer->GetNumVertices());
	ProjectVertexOnPlane(*vertexBuffer);
	FUVUtility::ShrinkUVsToBounds(NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	// Apply the calculated UVs to the StaticMesh
	FStaticMeshVertexBuffer* vertexBuffer = GetStaticMeshVertexBuffer();
	if (vertexBuffer != nullptr)
	{
		const int32 UVChannelIdx = 0;

		for (int32 vertexIndex = 0; vertexIndex < NewUVs.Num(); ++vertexIndex)
		{
			vertexBuffer->SetVertexUV(vertexIndex, UVChannelIdx, NewUVs[vertexIndex]);
		}

		StaticMesh->MarkPackageDirty();
	}
}

void FUVProjectionPlanarAlgo::PrepareUVs(int32 UVBufferSize)
{
	NewUVs.Empty(UVBufferSize);
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane(const FPositionVertexBuffer& PositionVertexBuffer)
{
	const int32 UVChannelIdx = 0;
	FVector2D newUV;

	for (uint32 vertexIndex = 0; vertexIndex < PositionVertexBuffer.GetNumVertices(); ++vertexIndex)
	{
		const FVector& vertexPosition = PositionVertexBuffer.VertexPosition(vertexIndex);
		newUV = Plane.ProjectToLocalCoordinates(vertexPosition);
		Inverse(newUV);
		NewUVs.Add(newUV);
	}
}

void FUVProjectionPlanarAlgo::InverseVertically(FVector2D& UV)
{
	UV.Y *= -1;
}

void FUVProjectionPlanarAlgo::SetPlane(const class FTransformablePlane& InPlane)
{
	Plane = InPlane;
}