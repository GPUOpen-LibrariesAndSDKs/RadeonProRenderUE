#pragma once

#include "IUVProjectionAlgorithm.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshVertexBuffer.h"
#include "ColorVertexBuffer.h"
#include "GCObject.h"
#include "SharedPointer.h"
#include "RawMesh.h"

/*
 * Abstraction class for UV projection algorithms
 */
class FUVProjectionAlgorithmBase : public IUVProjectionAlgorithm
{
public:
	
	struct FUVProjectionGlobalSettings
	{
		int32	UVChannel;

		FUVProjectionGlobalSettings();
	};

public:

	static bool	AreStaticMeshRenderDatasValid(UStaticMesh* InStaticMesh);

	virtual ~FUVProjectionAlgorithmBase() {}
	
	virtual void SetStaticMesh(class UStaticMesh* InStaticMesh) override;
	virtual FOnAlgorithmCompleted& OnAlgorithmCompleted() override;

	virtual void StartAlgorithm() override;
	virtual void AbortAlgorithm() override;
	virtual bool IsAlgorithimRunning() override;


protected:

	/* Clear the UV array and reserve space to fill it, based on the number of RawMesh.WedgeIndices. */
	void	PrepareUVs();

	void	StopAlgorithm();
	void	RaiseAlgorithmCompletion(bool bIsSuccess);
	void	StopAlgorithmAndRaiseCompletion(bool bIsSuccess);
	bool	AreStaticMeshRenderDatasValid() const;
	void	ApplyUVsOnMesh();
	void	SaveRawMesh();
	bool	IsTriangleAffectedByProjection(int32 TriangleIndex, int32& OutMaterialIndex) const;
	void	AddNewUVs(int32 MaterialIndex, const FVector2D& UV);
	void	FixInvalidUVsHorizontally();

	virtual const FUVProjectionGlobalSettings&	GetSettings() const = 0;


private:

	void	FixTextureCoordinateOnLeftSideIfRequired(float& TextureCoordinate);


protected:

	class UStaticMesh*		StaticMesh;
	FOnAlgorithmCompleted	OnAlgorithmCompletedEvent;
	FRawMesh				RawMesh;

private:

	bool		bIsAlgorithmRunning;

	typedef TMap<
		int32 /* MaterialIndex */,
		TArray<FVector2D> /* NewUVs */
	> TUVPerMaterialMap;

	TUVPerMaterialMap NewUVsPerMaterial;

};