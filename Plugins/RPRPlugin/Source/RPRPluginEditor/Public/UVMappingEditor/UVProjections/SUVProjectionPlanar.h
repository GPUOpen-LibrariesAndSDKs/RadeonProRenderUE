#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "IUVProjection.h"
#include "SharedPointer.h"
#include "IUVProjectionAlgorithm.h"
#include "SUVProjectionBase.h"
#include "ShapePreviewable.h"
#include "ShapePreviewPlane.h"

class SUVProjectionPlanar : public SUVProjectionBase, public FShapePreviewable<UShapePreviewPlane>
{
private:
	typedef FShapePreviewable<UShapePreviewPlane>	FShapePreviewPlane;

public:

	SLATE_BEGIN_ARGS(SUVProjectionPlanar) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual void	Release() override;
	
	virtual void	FinalizeCreation() override;

	FReply	Apply();
		
protected:

	virtual void InitializePostSetRPRStaticMeshEditor() override;

	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase*	GetShapePreview() override;

private:

	void	UpdateAlgorithmSettings();
	TSharedPtr<class FUVProjectionPlanarAlgo>	GetProjectionPlanarAlgo() const;

	void	CreateShapePreviewDetailView();

private:

	TSharedPtr<class IDetailsView> ShapePreviewDetailView;

};