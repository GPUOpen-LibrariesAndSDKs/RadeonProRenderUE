#pragma once

#include "UVProjectionAlgorithmBase.h"

class FUVProjectionCylinderAlgo : public FUVProjectionAlgorithmBase
{
public:

	struct FSettings
	{
		FVector	Center;
		FQuat	Rotation;
		float	Height;
	};

public:
	
	void	SetSettings(const FSettings& InSettings);

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;


private:

	void	ProjectVerticesToCylinder(int32 MeshIndex, int32 StartSection, int32 EndSection);
	void	ProjectVertexToCylinder(const FVector& Vertex, FVector2D& OutUV);


private:

	FSettings	Settings;

};
