#pragma once

#include "SharedPointer.h"
#include "Delegate.h"
#include "UVProjectionSettings.h"
#include "RPRMeshDataContainer.h"

class IUVProjectionAlgorithm : public TSharedFromThis<IUVProjectionAlgorithm>
{
public:

	virtual ~IUVProjectionAlgorithm() {}

	virtual void	SetMeshDatas(const FRPRMeshDataContainer& MeshDatas) = 0;
	virtual void	StartAlgorithm() = 0;
	virtual void	AbortAlgorithm() = 0;
	virtual bool	IsAlgorithimRunning() = 0;
	virtual void	Finalize() = 0;
	virtual void	SetGlobalUVProjectionSettings(FUVProjectionSettingsPtr Settings) = 0;
	
	DECLARE_EVENT_TwoParams(IUVProjectionAlgorithm, FOnAlgorithmCompleted, TSharedPtr<IUVProjectionAlgorithm> /* ProjectionInstance */ , bool /* Does algorithm succeed? */)
	virtual FOnAlgorithmCompleted&	OnAlgorithmCompleted() = 0;
	
};

typedef TSharedPtr<IUVProjectionAlgorithm> IUVProjectionAlgorithmPtr;