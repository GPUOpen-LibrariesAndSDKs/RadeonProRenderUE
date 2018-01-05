#include "UVProjectionPlanarAlgo.h"
#include "UVUtility.h"
#include "Engine/StaticMesh.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshVertexBuffer.h"
#include "RPRVectorTools.h"
#include "TransformablePlane.h"

FUVProjectionPlanarAlgo::FSettings::FSettings()
	: Plane(FPlane(FVector::ZeroVector, FVector::RightVector), FVector::ZeroVector, FVector::UpVector)
{

}

void FUVProjectionPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	const FPositionVertexBuffer* vertexBuffer = GetStaticMeshPositionVertexBuffer();
	if (vertexBuffer == nullptr)
	{
		StopAlgorithmAndRaiseCompletion(false);
		return;
	}

	PrepareUVs(NewUVs, vertexBuffer->GetNumVertices());
	ProjectVertexOnPlane(*vertexBuffer);
	FUVUtility::ShrinkUVsToBounds(NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane(const FPositionVertexBuffer& PositionVertexBuffer)
{
	const int32 UVChannelIdx = 0;
	FVector2D newUV;

	for (uint32 vertexIndex = 0; vertexIndex < PositionVertexBuffer.GetNumVertices(); ++vertexIndex)
	{
		const FVector& vertexPosition = PositionVertexBuffer.VertexPosition(vertexIndex);
		newUV = Settings.Plane.ProjectToLocalCoordinates(vertexPosition);
		FRPRVectorTools::InverseY(newUV);
		NewUVs.Add(newUV);
	}
}