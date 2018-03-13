#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewable.h"
#include "ShapePreviewCube.h"

/*
 * Widget displayed when the Cubic projection is selected
 */
class SUVProjectionCubic : public SUVProjectionBase, public FShapePreviewable<UShapePreviewCube>
{
private:
	typedef FShapePreviewable<UShapePreviewCube>		FShapePreviewCube;
	typedef TSharedPtr<class FUVProjectionCubicAlgo>	FUVProjectionCubicAlgoPtr;

public:

	SLATE_BEGIN_ARGS(SUVProjectionCubic) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void	FinalizeCreation();
	virtual void	OnUVProjectionDisplayed() override;
	virtual void	OnUVProjectionHidden() override;
	virtual void	OnPreAlgorithmStart() override;

protected:
	virtual void				OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase*	GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;

private:

	void						UpdateAlgorithmSettings();
	FUVProjectionCubicAlgoPtr	GetProjectionCubicAlgo() const;
	void						SetPreviewShapeSameBoundsAsShape();

private:

	TSharedPtr<class IDetailsView>	ShapePreviewDetailView;

};