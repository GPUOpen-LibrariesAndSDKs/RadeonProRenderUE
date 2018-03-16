#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewCylinder.h"
#include "ShapePreviewable.h"

class SUVProjectionCylinder : public SUVProjectionBase, public FShapePreviewable<UShapePreviewCylinder>
{
public:
	typedef FShapePreviewable<UShapePreviewCylinder>	FShapePreviewCylinder;

public:

	SLATE_BEGIN_ARGS(SUVProjectionCylinder)
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

	void	AdaptPreviewShapeToMesh();

	virtual void OnUVProjectionDisplayed() override;
	virtual void OnUVProjectionHidden() override;
	virtual void OnPreAlgorithmStart() override;


protected:
	
	virtual IUVProjectionAlgorithmPtr CreateAlgorithm() const override;
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase* GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;

private:

	void	UpdateAlgorithmSettings();

	TSharedPtr<class FUVProjectionCylinderAlgo>	GetProjectionCylinderAlgo() const;

private:

	TSharedPtr<IDetailsView>	ShapePreviewDetailView;

};
