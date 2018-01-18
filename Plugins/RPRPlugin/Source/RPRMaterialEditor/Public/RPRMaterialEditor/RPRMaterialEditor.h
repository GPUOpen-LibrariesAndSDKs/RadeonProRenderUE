#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"

extern const FName RPRMaterialInstanceEditorAppIdentifier;

class FRPRMaterialEditor : public FAssetEditorToolkit, FGCObject
{
public:

	void	InitRPRMaterialEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& TabManager) override;
	
	virtual bool IsPrimaryEditor() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	

private:

	void	InitMaterialEditorInstance(UMaterialInstanceConstant* InstanceConstant);

private:

	UMaterialEditorInstanceConstant*	MaterialEditorInstance;

};