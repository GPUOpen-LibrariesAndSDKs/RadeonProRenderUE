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

	void	StartCubicProjection(FRawMesh& InRawMesh, TArray<FVector2D>& OutNewUVs);


protected:

	virtual const FUVProjectionGlobalSettings& GetSettings() const override;


private:

	void	ProjectUVAlongAxis(TArray<FVector2D>& UVs, int32 VertexIndex, EAxis::Type AxisComponentA, EAxis::Type AxisComponentB);


private:

	TArray<FVector2D>	NewUVs;
	FSettings			Settings;

};