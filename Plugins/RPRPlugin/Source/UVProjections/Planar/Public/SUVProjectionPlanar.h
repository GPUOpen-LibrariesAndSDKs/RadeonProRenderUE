#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "IUVProjectionSettingsWidget.h"
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

	SLATE_BEGIN_ARGS(SUVProjectionPlanar) 
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void			Construct(const FArguments& InArgs);

	virtual void	OnUVProjectionDisplayed() override;
	virtual void	OnUVProjectionHidden() override;
	virtual void	OnPreAlgorithmStart() override;

protected:

	virtual IUVProjectionAlgorithmPtr CreateAlgorithm() const override;
	virtual void OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess) override;
	virtual UShapePreviewBase* GetShapePreview() override;
	virtual TSharedRef<SWidget> GetAlgorithmSettingsWidget() override;
	
private:

	void										UpdateAlgorithmSettings();
	TSharedPtr<class FUVProjectionPlanarAlgo>	GetProjectionPlanarAlgo() const;

private:

	TSharedPtr<class IDetailsView> ShapePreviewDetailView;

};