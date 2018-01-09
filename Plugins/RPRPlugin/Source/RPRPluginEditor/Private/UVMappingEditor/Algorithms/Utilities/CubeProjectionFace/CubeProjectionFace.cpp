#include "CubeProjectionFace.h"
#include "UVUtility.h"
#include "Class.h"

FCubeProjectionFace::FCubeProjectionFace(EUVProjectionFaceSide InProjectionFaceSide)
	: ProjectionFaceSide(InProjectionFaceSide)
{
	CreateProjectionPlane();
}

bool FCubeProjectionFace::AddVertexIndexIfOnFace(const FVector& VertexNormal, int32 VertexIndex)
{
	if (IsVertexOnFace(VertexNormal))
	{
		AddVertexIndex(VertexIndex);
		return (true);
	}
	return (false);
}

bool FCubeProjectionFace::IsVertexOnFace(const FVector& VertexNormal) const
{
	bool isPresentOnFace;
	const FVector absVertexNormal = VertexNormal.GetAbs();
	const bool bIsPositiveX = VertexNormal.X >= 0.0f;
	const bool bIsPositiveY = VertexNormal.Y >= 0.0f;
	const bool bIsPositiveZ = VertexNormal.Z >= 0.0f;

	switch (ProjectionFaceSide)
	{
	case PositiveX:
		isPresentOnFace = bIsPositiveX && IsOnFace_PlusX(absVertexNormal);
		break;
	case NegativeX:
		isPresentOnFace = !bIsPositiveX && IsOnFace_PlusX(absVertexNormal);
		break;
	case PositiveY:
		isPresentOnFace = bIsPositiveY && IsOnFace_PlusY(absVertexNormal);
		break;
	case NegativeY:
		isPresentOnFace = !bIsPositiveY && IsOnFace_PlusY(absVertexNormal);
		break;
	case PositiveZ:
		isPresentOnFace = bIsPositiveZ && IsOnFace_PlusZ(absVertexNormal);
		break;
	case NegativeZ:
		isPresentOnFace = !bIsPositiveZ && IsOnFace_PlusZ(absVertexNormal);
		break;

	default:
		isPresentOnFace = false;
		break;
	}

	return (isPresentOnFace);
}

FColor FCubeProjectionFace::GetFaceColor() const
{
	switch (ProjectionFaceSide)
	{
	case PositiveX:
		return (FColor::Red);
	case NegativeX:
		return (FColor(100, 0, 0));
	case PositiveY:
		return (FColor::Green);
	case NegativeY:
		return (FColor(0, 100, 0));
	case PositiveZ:
		return (FColor::Blue);
	case NegativeZ:
		return (FColor(0, 0, 100));

	default:
		return (FColor::Black);
	}
}

void FCubeProjectionFace::GetFaceProjectedUVs(const FPositionVertexBuffer& PositionVertexBuffer, TArray<FVector2D>& OutUVs) const
{
	int32 endOffset = OutUVs.Num();

	for (int32 i = 0; i < VertexIndexes.Num(); ++i)
	{
		const int32& vertexIndex = VertexIndexes[i];
		const FVector& positionVector = PositionVertexBuffer.VertexPosition(vertexIndex);
		OutUVs.Add(ProjectionPlane.ProjectToLocalCoordinates(positionVector));
	}

	FUVUtility::ShrinkUVsToBounds(OutUVs, endOffset);
}

const TArray<int32>& FCubeProjectionFace::GetFaceVertexIndexes() const
{
	return (VertexIndexes);
}

void FCubeProjectionFace::CreateProjectionPlane()
{
	FVector planeNormal = FVector::ForwardVector;
	FVector planeUp = FVector::UpVector;

	switch (ProjectionFaceSide)
	{
	case PositiveX:
		planeNormal = FVector::ForwardVector;
		planeUp = FVector::UpVector;
		break;
	case NegativeX:
		planeNormal = -FVector::ForwardVector;
		planeUp = -FVector::UpVector;
		break;
	case PositiveY:
		planeNormal = FVector::RightVector;
		planeUp = FVector::UpVector;
		break;
	case NegativeY:
		planeNormal = -FVector::RightVector;
		planeUp = -FVector::UpVector;
		break;
	case PositiveZ:
		planeNormal = FVector::UpVector;
		planeUp = FVector::RightVector;
		break;
	case NegativeZ:
		planeNormal = -FVector::UpVector;
		planeUp = -FVector::RightVector;
		break;

	default:
		break;
	}

	ProjectionPlane = FTransformablePlane(FPlane(planeNormal, 0), FVector::ZeroVector, planeUp);
}

void FCubeProjectionFace::AddVertexIndex(int32 VertexIndex)
{
	VertexIndexes.Add(VertexIndex);
}

EUVProjectionFaceSide FCubeProjectionFace::GetProjectionFaceSide() const
{
	return (ProjectionFaceSide);
}

const FTransformablePlane& FCubeProjectionFace::GetPlaneProjection() const
{
	return (ProjectionPlane);
}

FString FCubeProjectionFace::GetProjectionFaceSideName() const
{
	const UEnum* pEnum = FindObject<UEnum>((UObject*)ANY_PACKAGE, TEXT("ECubeProjectionFaceSide"));
	return (pEnum ? pEnum->GetNameStringByIndex(static_cast<int32>(ProjectionFaceSide)) : TEXT("unknown"));
}