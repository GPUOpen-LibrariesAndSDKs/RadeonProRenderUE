#include "UVProjectionCubicAlgo.h"
#include "RPRStaticMeshEditor.h"

void FUVProjectionCubicAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	const FPositionVertexBuffer* vertexBuffer = GetStaticMeshPositionVertexBuffer();
	if (vertexBuffer == nullptr)
	{
		StopAlgorithmAndRaiseCompletion(false);
		return;
	}

	StartCubicProjection(StaticMesh, Settings, NewUVs);

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCubicAlgo::Finalize()
{
	SetUVsOnMesh(NewUVs);
}

void FUVProjectionCubicAlgo::StartCubicProjection(UStaticMesh* InStaticMesh, const FSettings& InSettings, TArray<FVector2D>& OutNewUVs)
{
	TArray<FVector> reflectedVectors;
	CalculateReflectedVectors(InStaticMesh, InSettings, reflectedVectors);
	CalculateNewUVsFromReflectedVectors(reflectedVectors, OutNewUVs);
}

void FUVProjectionCubicAlgo::CalculateReflectedVectors(UStaticMesh* InStaticMesh, const FSettings& InSettings, TArray<FVector>& OutReflectedVectors)
{
	const FPositionVertexBuffer* positionVertexBuffer = GetStaticMeshPositionVertexBuffer(InStaticMesh);
	const FStaticMeshVertexBuffer* staticMeshVertexBuffer = GetStaticMeshVertexBuffer(InStaticMesh);
	FColorVertexBuffer* colorVertexBuffer = GetStaticMeshColorVertexBuffer(InStaticMesh);

	const FVector& cubeProjectionCenter = InSettings.CubeTransform.GetLocation();
	const FQuat& cubeProjectionRotation = InSettings.CubeTransform.GetRotation();

	OutReflectedVectors.Empty(positionVertexBuffer->GetNumVertices());

	TArray<FColor> colors;
	colors.Reserve(positionVertexBuffer->GetNumVertices());

	for (uint32 vertexIdx = 0; vertexIdx < positionVertexBuffer->GetNumVertices(); ++vertexIdx)
	{
		const FVector& vertexPosition = positionVertexBuffer->VertexPosition(vertexIdx);
		const FVector& vertexNormal = staticMeshVertexBuffer->VertexTangentZ(vertexIdx);
		
		const FVector localVertexPosition = vertexPosition; //cubeProjectionRotation.UnrotateVector(vertexPosition - cubeProjectionCenter);
		const FVector localVertexNormal = vertexNormal; //cubeProjectionRotation.UnrotateVector(vertexNormal);
		FVector reflectedVector = 2 * FVector::DotProduct(localVertexNormal, localVertexPosition) * localVertexNormal - localVertexPosition;
		reflectedVector.Normalize();

		OutReflectedVectors.Add(reflectedVector);
		colors.Add(FLinearColor(reflectedVector.X, reflectedVector.Y, reflectedVector.Z).ToFColor(true));
	}

	InSettings.RPRStaticMeshEditor->PaintStaticMeshPreview(colors);
}

void FUVProjectionCubicAlgo::CalculateNewUVsFromReflectedVectors(const TArray<FVector>& ReflectedVectors, TArray<FVector2D>& OutNewUVs)
{
	FVector2D uv;
	float max = 0;
	bool bIsSet = false;

	OutNewUVs.Empty(ReflectedVectors.Num());
	for (int32 idx = 0; idx < ReflectedVectors.Num(); ++idx)
	{
		const FVector& reflectedVector = ReflectedVectors[idx];

		if (IsOnFace_PlusX(reflectedVector))
		{
			uv.Set(-reflectedVector.Z, -reflectedVector.Y);
			max = FMath::Abs(reflectedVector.X);
			bIsSet = true;
		}
		else if (IsOnFace_MinusX(reflectedVector))
		{
			uv.Set(reflectedVector.Z, -reflectedVector.Y);
			max = FMath::Abs(reflectedVector.X);
		}
		else if (IsOnFace_PlusY(reflectedVector))
		{
			uv.Set(reflectedVector.X, reflectedVector.Z);
			max = FMath::Abs(reflectedVector.Y);
		}
		else if (IsOnFace_MinusY(reflectedVector))
		{
			uv.Set(reflectedVector.X, -reflectedVector.Z);
			max = FMath::Abs(reflectedVector.Y);
		}
		else if (IsOnFace_PlusZ(reflectedVector))
		{
			uv.Set(reflectedVector.X, -reflectedVector.Y);
			max = FMath::Abs(reflectedVector.Z);
		}
		else // if (IsOnFace_MinusZ(reflectedVector))
		{
			uv.Set(-reflectedVector.X, -reflectedVector.Y);
			max = FMath::Abs(reflectedVector.Z);
		}

		if (bIsSet)
		{
			OutNewUVs.Emplace((((uv.X ) + 1) / 2), (((uv.Y ) + 1) / 2));
		}
		else
		{
			OutNewUVs.Emplace(FVector2D::ZeroVector);
		}
	}
}

void FUVProjectionCubicAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}