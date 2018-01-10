#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "IUVProjection.h"
#include "SharedPointer.h"
#include "IUVProjectionAlgorithm.h"
#include "SUVProjectionBase.h"
#include "ShapePreviewable.h"
#include "ShapePreviewPlane.h"

/*
 * Widget displayed when the Planar projection is selected
 */
class SUVProjectionPlanar : public SUVProjectionBase, public FShapePreviewable<UShapePreviewPlane>
{
private:
	typedef FShapePreviewable<UShapePreviewPlane>	FShapePreviewPlane;

public:

	SLATE_BEGIN_ARGS(SUVProjectionPlanar) {}
	SLATE_END_ARGS()

	void			Construct(const FArguments& InArgs);

	virtual void	FinalizeCreation();
	virtual void	OnUVProjectionDisplayed() override;
	virtual void	OnUVProjectionHidden() override;

	void			ApplyAlgorithm();		


protected:

	virtual void				OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase*	GetShapePreview() override;

private:

	FReply										OnApplyButtonClicked();
	void										UpdateAlgorithmSettings();
	TSharedPtr<class FUVProjectionPlanarAlgo>	GetProjectionPlanarAlgo() const;

private:

	TSharedPtr<class IDetailsView> ShapePreviewDetailView;

};