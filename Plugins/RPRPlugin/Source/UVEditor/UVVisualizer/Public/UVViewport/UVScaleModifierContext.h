#pragma once
#include "Array.h"
#include "Vector2D.h"
#include "Vector.h"
#include "Transform.h"

class FUVScaleModifierContext
{
public:

	void	SetupContext(const TArray<FVector2D>& UVs, const FVector2D& Barycenter2D, const FTransform& InSceneTransform);
	void	ApplyScaleDelta(const FVector& Scale);

	const FVector2D		CalculateUV(int32 UVIndex) const;
	const FVector&		GetBarycenter() const;

private:

	FVector2D	To2D(const FVector& Point) const;
	FVector		To3D(const FVector2D& Point) const;

private:

	FVector				Barycenter3D;
	FVector				CumulatedScale;
	TArray<FVector2D>	InitialUV;
	FTransform			SceneTransform;

};
