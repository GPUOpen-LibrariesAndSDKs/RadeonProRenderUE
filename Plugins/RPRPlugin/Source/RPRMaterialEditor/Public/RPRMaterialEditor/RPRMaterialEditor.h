/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "RPRMaterialEditorInstanceConstant.h"
#include "Misc/NotifyHook.h"
#include "IDetailsView.h"
#include "RPRMaterial.h"

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

	URPRMaterial*						RPRMaterial;
	URPRMaterialEditorInstanceConstant*	MaterialEditorInstance;
	TSharedPtr<IDetailsView>			MaterialEditorInstanceDetailView;

};
