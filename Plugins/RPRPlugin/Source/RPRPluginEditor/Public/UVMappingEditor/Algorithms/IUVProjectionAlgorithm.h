#pragma once

#include "SharedPointer.h"
#include "Delegate.h"

class IUVProjectionAlgorithm
{
public:
	virtual void	SetStaticMesh(class UStaticMesh* StaticMesh) = 0;
	virtual void	StartAlgorithm() = 0;
	virtual void	AbortAlgorithm() = 0;
	virtual bool	IsAlgorithimRunning() = 0;
	virtual void	Finalize() = 0;
	
	DECLARE_EVENT_TwoParams(IUVProjectionAlgorithm, FOnAlgorithmCompleted, class IUVProjectionAlgorithm* /* ProjectionInstance */ , bool /* Does algorithm succeed? */)
	virtual FOnAlgorithmCompleted&	OnAlgorithmCompleted() = 0;
};

typedef TSharedPtr<IUVProjectionAlgorithm> IUVProjectionAlgorithmPtr;