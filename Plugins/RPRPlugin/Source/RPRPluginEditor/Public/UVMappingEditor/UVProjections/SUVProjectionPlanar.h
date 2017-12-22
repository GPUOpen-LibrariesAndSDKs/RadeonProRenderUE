#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "IUVProjection.h"
#include "SharedPointer.h"
#include "IUVProjectionAlgorithm.h"
#include "SUVProjectionBase.h"

class SUVProjectionPlanar : public SUVProjectionBase
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionPlanar) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void	FinalizeCreation() override;

	FReply	Apply();
	

protected:

	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithm* Algorithm, bool bIsSuccess) override;

};