#pragma once

#include "UVProjectionAlgorithmBase.h"

class FUVProjectionSphericalAlgo : public FUVProjectionAlgorithmBase
{
public:

	struct FSettings
	{
		FSettings();

		FVector	SphereCenter;
		FQuat	SphereRotation;
	};

public:

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

	void	ProjectVerticesOnSphere(TArray<FVector>& VertexPositions, TArray<uint32>& WedgeIndices);
	void	ProjectVertexOnSphere(const FVector& Vertex, FVector2D& OutUV);

	void	SetSettings(const FSettings& InSettings);
	
private:

	FSettings	Settings;

	int32		CurrentMeshIndex;
	int32		CurrentStartSection;
	int32		CurrentEndSection;
};