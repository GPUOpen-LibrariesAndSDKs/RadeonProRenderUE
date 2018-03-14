#pragma once

#ifdef UV_PROJECTION_CYLINDRICAL

#include "SUVProjectionBase.h"
#include "ShapePreviewCylinder.h"
#include "ShapePreviewable.h"

class SUVProjectionCylinder : public SUVProjectionBase, public FShapePreviewable<UShapePreviewCylinder>
{
public:
	typedef FShapePreviewable<UShapePreviewCylinder>	FShapePreviewCylinder;

public:

	SLATE_BEGIN_ARGS(SUVProjectionCylinder)
		: _StaticMesh(nullptr)
	{}

		SLATE_ARGUMENT(UStaticMesh*, StaticMesh)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

	void	AdaptPreviewShapeToMesh();

	virtual void FinalizeCreation() override;
	virtual void OnUVProjectionDisplayed() override;
	virtual void OnUVProjectionHidden() override;
	virtual void OnPreAlgorithmStart() override;


protected:
	
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase* GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;

private:

	void	UpdateAlgorithmSettings();

	TSharedPtr<class FUVProjectionCylinderAlgo>	GetProjectionCylinderAlgo() const;

private:

	TSharedPtr<IDetailsView>	ShapePreviewDetailView;

};

#endif