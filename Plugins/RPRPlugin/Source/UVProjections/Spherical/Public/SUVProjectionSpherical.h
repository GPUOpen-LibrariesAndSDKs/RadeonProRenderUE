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

	SLATE_BEGIN_ARGS(SUVProjectionSpherical)
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

	virtual IUVProjectionAlgorithmPtr CreateAlgorithm(const TArray<class UStaticMesh*>& StaticMeshes) override;
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase* GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;

private:

	void	UpdateAlgorithmSettings();
	
	TSharedPtr<class FUVProjectionSphericalAlgo>	GetProjectionSphericalAlgo() const;

private:

	TSharedPtr<IDetailsView>	ShapePreviewDetailView;

};
