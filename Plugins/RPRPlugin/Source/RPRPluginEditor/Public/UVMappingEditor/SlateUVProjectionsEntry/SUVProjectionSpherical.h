#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewable.h"
#include "ShapePreviewSphere.h"

/*
 * Widget displayed when the Spherical projection is selected
 */
class SUVProjectionSpherical : public SUVProjectionBase, public FShapePreviewable<UShapePreviewSphere>
{
public:
	typedef FShapePreviewable<UShapePreviewSphere> FShapePreviewSphere;

public:

	SLATE_BEGIN_ARGS(SUVProjectionSpherical) {}
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
	
	TSharedPtr<class FUVProjectionSphericalAlgo>	GetProjectionSphericalAlgo() const;

private:

	TSharedPtr<IDetailsView>	ShapePreviewDetailView;

};

