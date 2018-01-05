#pragma once

#include "UVProjectionAlgorithmBase.h"


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

	void	StartCubicProjection(UStaticMesh* InStaticMesh, const FUVProjectionCubicAlgo::FSettings& InSettings, TArray<FVector2D>& OutNewUVs);

private:

	void CalculateReflectedVectors(UStaticMesh* InStaticMesh, const FUVProjectionCubicAlgo::FSettings& InSettings, TArray<FVector>& OutReflectedVectors);
	void CalculateNewUVsFromReflectedVectors(const TArray<FVector>& ReflectedVectors, TArray<FVector2D>& OutNewUVs);

	static inline bool IsOnFace_PlusX(const FVector& ReflectedVector)	{ return (ReflectedVector.X >= ReflectedVector.Y && ReflectedVector.X >= ReflectedVector.Z); }
	static inline bool IsOnFace_MinusX(const FVector& ReflectedVector)	{ return (ReflectedVector.X <= ReflectedVector.Y && ReflectedVector.X <= ReflectedVector.Z); }

	static inline bool IsOnFace_PlusY(const FVector& ReflectedVector)	{ return (ReflectedVector.Y >= ReflectedVector.Z && ReflectedVector.Y >= ReflectedVector.X); }
	static inline bool IsOnFace_MinusY(const FVector& ReflectedVector)	{ return (ReflectedVector.Y <= ReflectedVector.Z && ReflectedVector.Y <= ReflectedVector.X); }

	static inline bool IsOnFace_PlusZ(const FVector& ReflectedVector)	{ return (ReflectedVector.Z >= ReflectedVector.Y && ReflectedVector.Z >= ReflectedVector.X); }
	static inline bool IsOnFace_MinusZ(const FVector& ReflectedVector)	{ return (ReflectedVector.Z <= ReflectedVector.Y && ReflectedVector.Z <= ReflectedVector.X); }

private:

	TArray<FVector2D>	NewUVs;
	FSettings			Settings;

};