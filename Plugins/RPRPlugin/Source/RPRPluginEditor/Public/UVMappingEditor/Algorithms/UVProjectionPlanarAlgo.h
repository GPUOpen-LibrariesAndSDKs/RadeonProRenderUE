#pragma once

#include "UVProjectionAlgorithmBase.h"
#include "TransformablePlane.h"

class FUVProjectionPlanarAlgo : public FUVProjectionAlgorithmBase
{
public:
	struct FSettings : public FUVProjectionAlgorithmBase::FUVProjectionGlobalSettings
	{
		FSettings();

		FTransformablePlane Plane;
	};

public:

	void	SetSettings(const FSettings& InSettings);

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

private:

	void	ProjectVertexOnPlane(const class FPositionVertexBuffer& VertexBuffer);

private:

	TArray<FVector2D>	NewUVs;
	FSettings			Settings;

};