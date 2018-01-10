#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewCylinder.h"
#include "ShapePreviewable.h"

class SUVProjectionCylinder : public SUVProjectionBase, public FShapePreviewable<UShapePreviewCylinder>
{
public:
	typedef FShapePreviewable<UShapePreviewCylinder>	FShapePreviewCylinder;

public:

	SLATE_BEGIN_ARGS(SUVProjectionCylinder) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	ApplyAlgorithm();

	void	AdaptPreviewShapeToMesh();

	virtual void FinalizeCreation() override;
	virtual void OnUVProjectionDisplayed() override;
	virtual void OnUVProjectionHidden() override;


protected:
	
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase* GetShapePreview() override;

private:

	FReply	OnApplyButtonClicked();
	void	UpdateAlgorithmSettings();

	TSharedPtr<class FUVProjectionCylinderAlgo>	GetProjectionCylinderAlgo() const;

private:

	TSharedPtr<IDetailsView>	ShapePreviewDetailView;

};