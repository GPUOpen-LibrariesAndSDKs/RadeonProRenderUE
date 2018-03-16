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

	SLATE_BEGIN_ARGS(SUVProjectionCubic)
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	virtual void	OnUVProjectionDisplayed() override;
	virtual void	OnUVProjectionHidden() override;
	virtual void	OnPreAlgorithmStart() override;

protected:

	virtual IUVProjectionAlgorithmPtr CreateAlgorithm() const override;
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase*	GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;

private:

	void						UpdateAlgorithmSettings();
	FUVProjectionCubicAlgoPtr	GetProjectionCubicAlgo() const;
	void						SetPreviewShapeSameBoundsAsShape();

private:

	TSharedPtr<class IDetailsView>	ShapePreviewDetailView;

};
