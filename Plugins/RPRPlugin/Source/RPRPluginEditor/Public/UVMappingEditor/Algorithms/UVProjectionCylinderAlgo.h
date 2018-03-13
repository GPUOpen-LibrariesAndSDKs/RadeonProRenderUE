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

	void	ProjectVerticesToCylinder(const TArray<FVector>& Vertices, const TArray<uint32>& Triangles);
	void	ProjectVertexToCylinder(const FVector& Vertex, FVector2D& OutUV);


private:

	FSettings	Settings;

};