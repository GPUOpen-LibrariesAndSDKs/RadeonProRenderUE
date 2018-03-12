#include "ShapePreviewCube.h"
#include "DrawDebugHelpers.h"

UShapePreviewCube::UShapePreviewCube()
	: Thickness(1.0f)
	, FaceScale(0.9f)
	, FaceOffset(0.5f)
	, ArrowHeadSize(20.0f)
	, ArrowExtentSize(40.0f)
{
	CubeProjectionFaces = FCubeProjectionFace::CreateAllCubeProjectionFaces();
}

void UShapePreviewCube::DrawShapePreview()
{
	const float lifeTime = -1.0f;
	const bool bPersistentLines = false;
	const uint8 depthPriority = 0;

	DrawDebugBox(
		GetWorld(),
		GetComponentLocation(),
		GetComponentScale(),
		GetComponentRotation().Quaternion(),
		GetShapeColor(),
		bPersistentLines,
		lifeTime,
		depthPriority,
		Thickness
	);

	//for (int32 i = 0; i < CubeProjectionFaces.Num(); ++i)
	//{
	//	FColor faceColor = CubeProjectionFaces[i].GetFaceColor();
	//	const FTransformablePlane& plane = CubeProjectionFaces[i].GetPlaneProjection();

	//	FVector boxSideCenter = GetComponentLocation() + (plane.GetPlaneNormal() + plane.GetPlaneNormal() * FaceOffset) * GetComponentScale();
	//	FVector arrowStart = boxSideCenter + plane.GetPlaneNormal() * 0.1f;
	//	
	//	DrawDebugSolidBox(
	//		GetWorld(),
	//		boxSideCenter,
	//		(plane.GetUp() + plane.GetLeft()).GetAbs() * GetComponentScale() * FaceScale,
	//		faceColor,
	//		bPersistentLines,
	//		lifeTime,
	//		depthPriority
	//	);

	//	// Normal

	//	DrawDebugDirectionalArrow(
	//		GetWorld(),
	//		arrowStart,
	//		arrowStart + plane.GetPlaneNormal() * ArrowExtentSize,
	//		ArrowHeadSize,
	//		FColor::Red
	//		);

	//	// Up

	//	DrawDebugDirectionalArrow(
	//		GetWorld(),
	//		arrowStart,
	//		arrowStart + plane.GetUp() * ArrowExtentSize,
	//		ArrowHeadSize,
	//		FColor::Blue
	//	);

	//	// Right

	//	DrawDebugDirectionalArrow(
	//		GetWorld(),
	//		arrowStart,
	//		arrowStart + plane.GetLeft() * ArrowExtentSize,
	//		ArrowHeadSize,
	//		FColor::Green
	//	);
	//}
}

