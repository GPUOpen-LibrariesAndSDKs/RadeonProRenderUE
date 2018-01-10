#pragma once

#include "PackVertexUV.h"
#include "CubeProjectionFace.h"
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

	void	StartCubicProjection(FRawMesh& InRawMesh, 
							const FUVProjectionCubicAlgo::FSettings& InSettings,
							TArray<FVector2D>& OutNewUVs);

private:

	static void PutVertexIntoCubeProjectionFaceByNormals(const FSettings& InSettings, const FRawMesh& InRawMesh, FCubeProjectionFaces& OutProjectionFaces);
	static void ProjectCubeFaceToUVs(const FRawMesh& InRawMesh, const TArray<FCubeProjectionFace>& CubeProjectionFaces, TArray<FVector2D>& OutUVs);

	static inline float ClampTextureCoordinateToBounds(float TextureCoordinate, float Max) { return (((TextureCoordinate / Max) + 1) / 2); }

private:

	TArray<FVector2D>	NewUVs;
	FSettings			Settings;

};