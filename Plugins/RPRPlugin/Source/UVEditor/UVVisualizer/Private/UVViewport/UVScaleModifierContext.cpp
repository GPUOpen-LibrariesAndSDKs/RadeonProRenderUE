#include "UVScaleModifierContext.h"
#include "UVUtility.h"

DECLARE_LOG_CATEGORY_CLASS(UVScaleModifierContextLog, Log, All);

void FUVScaleModifierContext::SetupContext(const TArray<FVector2D>& UVs, const FVector2D& Barycenter2D, const FTransform& InSceneTransform)
{
	CumulatedScale = FVector::ZeroVector;
	InitialUV = UVs;
	SceneTransform = InSceneTransform;

	Barycenter3D = To3D(Barycenter2D);
}

void FUVScaleModifierContext::ApplyScaleDelta(const FVector& Scale)
{
	CumulatedScale += Scale;

	UE_LOG(UVScaleModifierContextLog, Log, TEXT("Scale : %s (+%s)"), *CumulatedScale.ToCompactString(), *Scale.ToCompactString());
}

const FVector2D	FUVScaleModifierContext::CalculateUV(int32 UVIndex) const
{
	const FVector2D& uv = InitialUV[UVIndex];

	FVector uv3D = To3D(uv);
	FVector direction = (uv3D - Barycenter3D);
	direction.Y = 0;
	FVector newUV3D = uv3D + direction * CumulatedScale;

	return (To2D(newUV3D));
}

const FVector& FUVScaleModifierContext::GetBarycenter() const
{
	return (Barycenter3D);
}

FVector2D FUVScaleModifierContext::To2D(const FVector& Point) const
{
	FVector transformedPoint = SceneTransform.InverseTransformPosition(Point);
	return (FVector2D(transformedPoint.X, transformedPoint.Z));
}

FVector FUVScaleModifierContext::To3D(const FVector2D& Point) const
{
	return (SceneTransform.TransformPosition(FVector(Point.X, 0, Point.Y)));
}

