#pragma once

#include "IUVProjection.h"
#include "DeclarativeSyntaxSupport.h"
#include "UVProjectionType.h"
#include "IUVProjectionAlgorithm.h"
#include "SCompoundWidget.h"
#include "GCObject.h"

/*
 * Abstract class for UV Projection widgets
 */
class SUVProjectionBase : public SCompoundWidget, public IUVProjection
{
public:
	SLATE_BEGIN_ARGS(SUVProjectionBase) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

	virtual void				SetStaticMesh(class UStaticMesh* InStaticMesh) override;
	virtual UStaticMesh*		GetStaticMesh() const override;
	virtual TSharedRef<SWidget> TakeWidget() override;

protected:
	
	void	ConstructBase();

	void	InitializeAlgorithm(EUVProjectionType ProjectionType);
	void	StartAlgorithm();
	void	FinalizeAlgorithm();

	virtual void	OnAlgorithmCompleted(IUVProjectionAlgorithm* Algorithm, bool bIsSuccess) = 0;

private:

	void	SubscribeToAlgorithmCompletion();

private:

	IUVProjectionAlgorithmPtr	algorithm;
	UStaticMesh*				staticMesh;

};
