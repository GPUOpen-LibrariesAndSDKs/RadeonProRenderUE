#pragma once

#include "UVProjectionAlgorithmBase.h"
#include "TransformablePlane.h"

class FUVProjectionPlanarAlgo : public FUVProjectionAlgorithmBase
{
public:
	struct FSettings
	{
		FSettings();

		FTransformablePlane Plane;
		float	Scale;
	};

public:

	void	SetSettings(const FSettings& InSettings);

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

private:

	void	ProjectVertexOnPlane(int32 MeshIndex);
	

private:

	FSettings			Settings;


};
