#pragma once

#include "UVProjectionAlgorithmBase.h"
#include "Axis.h"

class FUVProjectionCubicAlgo : public FUVProjectionAlgorithmBase
{
public:
	struct FSettings : public FUVProjectionAlgorithmBase::FUVProjectionGlobalSettings
	{
		FTransform	CubeTransform;
	};

public:

	void	SetSettings(const FSettings& InSettings);
	
	virtual void StartAlgorithm() override;
	virtual void Finalize() override;

	void	StartCubicProjection(FRawMesh& InRawMesh);


protected:

	virtual const FUVProjectionGlobalSettings& GetSettings() const override;


private:

	void	ProjectUVAlongAxis(int32 MaterialIndex, int32 VertexIndex,
					EAxis::Type AxisComponentA, EAxis::Type AxisComponentB);
	
private:

	FSettings			Settings;

};