#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewBase.h"
#include "TriPlanarSettings.h"
#include "IDetailsView.h"

/*
 * Widget displayed when the TriPlanar projection is selected
 */
class SUVProjectionTriPlanar : public SUVProjectionBase, public FGCObject
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionTriPlanar)
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
		
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

protected:

	virtual IUVProjectionAlgorithmPtr	CreateAlgorithm() const;
	virtual TSharedRef<SWidget>			GetAlgorithmSettingsWidget();
	virtual void						OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess);
	virtual UShapePreviewBase*			GetShapePreview();
	
	virtual void OnPreAlgorithmStart() override;

private:

	void	InitTriPlanarSettings();
	void	UpdateAlgorithmSettings();

private:

	UTriPlanarSettings*			Settings;
	TSharedPtr<IDetailsView>	SettingsDetailsView;

};
