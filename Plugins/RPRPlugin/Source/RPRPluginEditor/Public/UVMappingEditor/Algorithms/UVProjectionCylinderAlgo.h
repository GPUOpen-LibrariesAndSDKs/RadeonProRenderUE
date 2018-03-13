#pragma once

#include "UVProjectionAlgorithmBase.h"

class FUVProjectionCylinderAlgo : public FUVProjectionAlgorithmBase
{
public:

	struct FSettings : public FUVProjectionAlgorithmBase::FUVProjectionGlobalSettings
	{
		FVector	Center;
		FQuat	Rotation;
		float	Height;
	};

public:
	
	void	SetSettings(const FSettings& InSettings);

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

	static void	ProjectVerticesToCylinder(const FSettings& InSettings, const TArray<FVector>& Vertices, 
										const TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs);

	static void	ProjectVertexToCylinder(const FSettings& InSettings, const FVector& Vertex, FVector2D& OutUV);


protected:

	virtual const FUVProjectionGlobalSettings& GetSettings() const override;


private:

	TArray<FVector2D>	NewUVs;
	FSettings	Settings;

};