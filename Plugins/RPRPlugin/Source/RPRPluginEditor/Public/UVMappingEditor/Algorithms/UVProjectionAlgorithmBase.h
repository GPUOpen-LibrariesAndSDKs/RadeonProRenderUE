#pragma once

#include "IUVProjectionAlgorithm.h"
#include "GCObject.h"

/*
 * Abstraction class for UV projection algorithms
 */
class FUVProjectionAlgorithmBase : public IUVProjectionAlgorithm
{
public:

	virtual ~FUVProjectionAlgorithmBase() {}

	virtual void SetStaticMesh(class UStaticMesh* InStaticMesh) override;
	virtual FOnAlgorithmCompleted& OnAlgorithmCompleted() override;

	virtual void StartAlgorithm() override;
	virtual void AbortAlgorithm() override;
	virtual bool IsAlgorithimRunning() override;

protected:

	void	StopAlgorithm();
	void	RaiseAlgorithmCompletion(bool bIsSuccess);
	void	StopAlgorithmAndRaiseCompletion(bool bIsSuccess);
	bool	AreStaticMeshRenderDatasValid() const;

	class FPositionVertexBuffer*	GetStaticMeshPositionVertexBuffer() const;
	class FStaticMeshVertexBuffer*	GetStaticMeshVertexBuffer() const;

protected:

	class UStaticMesh*			StaticMesh;
	FOnAlgorithmCompleted		OnAlgorithmCompletedEvent;

private:

	bool	bIsAlgorithmRunning;

};