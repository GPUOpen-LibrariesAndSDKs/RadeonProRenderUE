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
		float	Scale;
	};

public:

	void	SetSettings(const FSettings& InSettings);

	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

	void	ProjectVertexOnPlane();
	

protected:

	virtual const FUVProjectionGlobalSettings& GetSettings() const override;


private:

	TArray<FVector2D>	NewUVs;
	FSettings			Settings;


};