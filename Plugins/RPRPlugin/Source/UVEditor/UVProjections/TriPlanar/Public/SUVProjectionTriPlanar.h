#pragma once

#include "SUVProjectionBase.h"
#include "ShapePreviewBase.h"
#include "TriPlanarSettings.h"
#include "IDetailsView.h"
#include "NotifyHook.h"
#include "IStructureDetailsView.h"

/*
 * Widget displayed when the TriPlanar projection is selected
 */
class SUVProjectionTriPlanar : public SUVProjectionBase, public FNotifyHook
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionTriPlanar)
		: _RPRStaticMeshEditorPtr()
	{}

		SLATE_ARGUMENT(FRPRStaticMeshEditorWeakPtr, RPRStaticMeshEditorPtr)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
		
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

	FTriPlanarSettings			        Settings;
	TSharedPtr<IStructureDetailsView>	SettingsDetailsView;

};
