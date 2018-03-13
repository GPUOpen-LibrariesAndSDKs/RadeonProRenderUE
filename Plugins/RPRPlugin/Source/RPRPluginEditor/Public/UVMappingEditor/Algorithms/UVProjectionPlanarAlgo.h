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

	static void	ProjectVertexOnPlane(const FSettings& InSettings, const TArray<FVector>& VertexPositions, const TArray<uint32>& WedgeIndices, TArray<FVector2D>& OutUVs);
	

protected:

	virtual const FUVProjectionGlobalSettings& GetSettings() const override;


private:

	TArray<FVector2D>	NewUVs;
	FSettings			Settings;


};