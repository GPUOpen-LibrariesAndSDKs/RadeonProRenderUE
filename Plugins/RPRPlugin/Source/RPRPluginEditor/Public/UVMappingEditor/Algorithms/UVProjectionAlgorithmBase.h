#pragma once

#include "IUVProjectionAlgorithm.h"
#include "PositionVertexBuffer.h"
#include "StaticMeshVertexBuffer.h"
#include "ColorVertexBuffer.h"
#include "GCObject.h"
#include "SharedPointer.h"

/*
 * Abstraction class for UV projection algorithms
 */
class FUVProjectionAlgorithmBase : public IUVProjectionAlgorithm
{
public:
	
	struct FUVProjectionGlobalSettings
	{
		// Quick access for debug. Should be removed later.
		TSharedPtr<class FRPRStaticMeshEditor> RPRStaticMeshEditor;
	};

public:

	virtual ~FUVProjectionAlgorithmBase() {}

	virtual void SetStaticMesh(class UStaticMesh* InStaticMesh) override;
	virtual FOnAlgorithmCompleted& OnAlgorithmCompleted() override;

	virtual void StartAlgorithm() override;
	virtual void AbortAlgorithm() override;
	virtual bool IsAlgorithimRunning() override;

	static bool							AreStaticMeshRenderDatasValid(UStaticMesh* InStaticMesh);
	static FPositionVertexBuffer*		GetStaticMeshPositionVertexBuffer(UStaticMesh* InStaticMesh);
	static FStaticMeshVertexBuffer*		GetStaticMeshVertexBuffer(UStaticMesh* InStaticMesh);
	static FColorVertexBuffer*			GetStaticMeshColorVertexBuffer(UStaticMesh* InStaticMesh);

protected:

	/* Clear the UV array and reserve space to fill it */
	void	PrepareUVs(TArray<FVector2D>& UVs, int32 UVBufferSize);

	void	StopAlgorithm();
	void	RaiseAlgorithmCompletion(bool bIsSuccess);
	void	StopAlgorithmAndRaiseCompletion(bool bIsSuccess);
	bool	AreStaticMeshRenderDatasValid() const;
	void	SetUVsOnMesh(const TArray<FVector2D>& UVs);

	FPositionVertexBuffer*		GetStaticMeshPositionVertexBuffer() const;
	FStaticMeshVertexBuffer*	GetStaticMeshVertexBuffer() const;
	FColorVertexBuffer*			GetStaticMeshColorVertexBuffer() const;

protected:

	class UStaticMesh*			StaticMesh;
	FOnAlgorithmCompleted		OnAlgorithmCompletedEvent;

private:

	bool	bIsAlgorithmRunning;

};