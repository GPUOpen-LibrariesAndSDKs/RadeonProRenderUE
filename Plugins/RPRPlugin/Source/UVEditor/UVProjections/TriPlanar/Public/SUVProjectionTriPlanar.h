#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewBase.h"
#include "TriPlanarSettings.h"
#include "IDetailsView.h"
#include "NotifyHook.h"

/*
 * Widget displayed when the TriPlanar projection is selected
 */
class SUVProjectionTriPlanar : public SUVProjectionBase, public FNotifyHook, public FGCObject
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionTriPlanar)
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
		
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual void OnSectionSelectionChanged() override;

	/* FNotifyHook implementation */
	virtual void NotifyPreChange(class FEditPropertyChain* PropertyAboutToChange) {}
	/* Called when the settings changed */
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, class FEditPropertyChain* PropertyThatChanged);

protected:

	virtual IUVProjectionAlgorithmPtr	CreateAlgorithm() const;
	virtual TSharedRef<SWidget>			GetAlgorithmSettingsWidget();
	virtual void						OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess);
	virtual UShapePreviewBase*			GetShapePreview();
	
	virtual void OnPreAlgorithmStart() override;
	virtual bool RequiredManualApply() const override;

private:

	void	InitTriPlanarSettings();
	void	TryLoadTriPlanarSettings();
	void	UpdateAlgorithmSettings();

private:

	UTriPlanarSettings*			Settings;
	TSharedPtr<IDetailsView>	SettingsDetailsView;

};
