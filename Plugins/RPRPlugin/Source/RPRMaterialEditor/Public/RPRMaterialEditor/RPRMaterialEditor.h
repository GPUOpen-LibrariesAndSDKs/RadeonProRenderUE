#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "RPRMaterialEditorInstanceConstant.h"
#include "Misc/NotifyHook.h"
#include "IDetailsView.h"


class FRPRMaterialEditor : public FAssetEditorToolkit, FGCObject, FNotifyHook
{
public:

	static const FName	RPRMaterialInstanceEditorAppIdentifier;
	static const FName	PropertiesTabId;
	
public:

	void	InitRPRMaterialEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit);

	TSharedRef<FTabManager::FLayout>	GenerateDefaultLayout() const;

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	
	virtual bool IsPrimaryEditor() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;

private:

	void	InitMaterialEditorInstance(UMaterialInstanceConstant* InstanceConstant);
	void	InitPropertyDetailsView(UMaterialInstanceConstant* InstanceConstant);

	TSharedRef<SDockTab>	SpawnTab_Properties(const FSpawnTabArgs& Args);

private:

	URPRMaterialEditorInstanceConstant*	MaterialEditorInstance;
	TSharedPtr<IDetailsView>			MaterialEditorInstanceDetailView;

};