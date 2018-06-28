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
