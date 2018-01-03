#pragma once

#include "UVProjectionAlgorithmBase.h"
#include "TransformablePlane.h"

class FUVProjectionPlanarAlgo : public FUVProjectionAlgorithmBase
{
public:

	FUVProjectionPlanarAlgo();

	void	SetPlane(const class FTransformablePlane& InPlane);

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

private:

	void	PrepareUVs(int32 UVBufferSize);
	void	ProjectVertexOnPlane(const class FPositionVertexBuffer& VertexBuffer);
	void	InverseVertically(FVector2D& UV);

private:

	TArray<FVector2D>	NewUVs;
	FTransformablePlane	Plane;

};