#include "CubeProjectionFace.h"
#include "UVUtility.h"
#include "Class.h"

FCubeProjectionFaces FCubeProjectionFace::CreateAllCubeProjectionFaces()
{
	FCubeProjectionFaces faces;

	faces.Emplace(EUVProjectionFaceSide::PositiveX);
	faces.Emplace(EUVProjectionFaceSide::NegativeX);
	faces.Emplace(EUVProjectionFaceSide::PositiveY);
	faces.Emplace(EUVProjectionFaceSide::NegativeY);
	faces.Emplace(EUVProjectionFaceSide::PositiveZ);
	faces.Emplace(EUVProjectionFaceSide::NegativeZ);

	return (faces);
}

FCubeProjectionFace::FCubeProjectionFace(EUVProjectionFaceSide InProjectionFaceSide)
	: ProjectionFaceSide(InProjectionFaceSide)
{
	CreateProjectionPlane();
}

bool FCubeProjectionFace::AddUVIndexIfVertexIsOnFace(const FVector& VertexNormal, int32 UVIndex)
{
	if (IsVertexOnFace(VertexNormal))
	{
		AddUVIndex(UVIndex);
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
		return (FColor(130, 0, 0));
	case NegativeX:
		return (FColor(100, 0, 0));
	case PositiveY:
		return (FColor(0, 130, 0));
	case NegativeY:
		return (FColor(0, 100, 0));
	case PositiveZ:
		return (FColor(0, 0, 130));
	case NegativeZ:
		return (FColor(0, 0, 100));

	default:
		return (FColor::Black);
	}
}

void FCubeProjectionFace::GetFaceProjectedUVs(const TArray<FVector>& Vertices, const TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs) const
{
	FVector2D uv;
	int32 endOffset = OutUVs.Num();

	for (int32 i = 0; i < UVIndexes.Num(); ++i)
	{
		const int32 uvIndex = UVIndexes[i];
		const int32 vertexIndice = WedgeIndices[uvIndex];
		const FVector& positionVector = Vertices[vertexIndice];
		uv = ProjectionPlane.ProjectToLocalCoordinates(positionVector);
		//FUVUtility::InvertUV(uv);
		OutUVs.Add(uv);
	}

	FUVUtility::ShrinkUVsToBounds(OutUVs, endOffset);
}

const TArray<int32>& FCubeProjectionFace::GetFaceVertexWedgeIndiceIndexes() const
{
	return (UVIndexes);
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
		planeUp = FVector::UpVector;
		break;
	case PositiveY:
		planeNormal = FVector::RightVector;
		planeUp = FVector::UpVector;
		break;
	case NegativeY:
		planeNormal = -FVector::RightVector;
		planeUp = FVector::UpVector;
		break;
	case PositiveZ:
		planeNormal = FVector::UpVector;
		planeUp = -FVector::ForwardVector;
		break;
	case NegativeZ:
		planeNormal = -FVector::UpVector;
		planeUp = FVector::ForwardVector;
		break;

	default:
		break;
	}

	ProjectionPlane = FTransformablePlane(FPlane(planeNormal, 0), FVector::ZeroVector, planeUp);
}

void FCubeProjectionFace::AddUVIndex(int32 UVIndex)
{
	UVIndexes.Add(UVIndex);
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
	const UEnum* pEnum = FindObject<UEnum>((UObject*)ANY_PACKAGE, TEXT("EUVProjectionFaceSide"));
	return (pEnum ? pEnum->GetNameStringByIndex(static_cast<int32>(ProjectionFaceSide)) : TEXT("unknown"));
}