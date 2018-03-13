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

	virtual ~FUVProjectionAlgorithmBase() {}

	virtual void SetStaticMesh(class UStaticMesh* InStaticMesh) override;
	virtual FOnAlgorithmCompleted& OnAlgorithmCompleted() override;

	virtual void StartAlgorithm() override;
	virtual void AbortAlgorithm() override;
	virtual bool IsAlgorithimRunning() override;

	static bool							AreStaticMeshRenderDatasValid(UStaticMesh* InStaticMesh);

protected:

	/* Clear the UV array and reserve space to fill it, based on the number of RawMesh.WedgeIndices. */
	void	PrepareUVs(TArray<FVector2D>& UVs);

	void	StopAlgorithm();
	void	RaiseAlgorithmCompletion(bool bIsSuccess);
	void	StopAlgorithmAndRaiseCompletion(bool bIsSuccess);
	bool	AreStaticMeshRenderDatasValid() const;
	void	SetUVsOnMesh(const TArray<FVector2D>& UVs);
	void	SaveRawMesh();

	virtual const FUVProjectionGlobalSettings&	GetSettings() const = 0;

protected:

	class UStaticMesh*		StaticMesh;
	FOnAlgorithmCompleted	OnAlgorithmCompletedEvent;
	FRawMesh				RawMesh;

private:

	bool		bIsAlgorithmRunning;

};