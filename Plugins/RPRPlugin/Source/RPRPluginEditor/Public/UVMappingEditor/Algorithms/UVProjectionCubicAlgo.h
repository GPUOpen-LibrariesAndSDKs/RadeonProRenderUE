#pragma once

#ifdef UV_PROJECTION_CUBIC

#include "UVProjectionAlgorithmBase.h"
#include "Axis.h"

class FUVProjectionCubicAlgo : public FUVProjectionAlgorithmBase
{
public:
	struct FSettings
	{
		FTransform	CubeTransform;
	};

public:

	void	SetSettings(const FSettings& InSettings);
	
	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

	void	StartCubicProjection(FRawMesh& InRawMesh);
	

private:

	void	ProjectUVAlongAxis(int32 VertexIndex, EAxis::Type AxisComponentA, EAxis::Type AxisComponentB);
	
private:

	FSettings			Settings;

};

#endif