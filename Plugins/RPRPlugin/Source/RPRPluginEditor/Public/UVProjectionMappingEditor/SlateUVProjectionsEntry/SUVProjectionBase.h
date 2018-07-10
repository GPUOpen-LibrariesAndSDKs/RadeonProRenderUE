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

#include "UVProjectionMappingEditor/SlateUVProjectionsEntry/IUVProjectionSettingsWidget.h"
#include "Engine/StaticMesh.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "UVProjectionMappingEditor/UVProjectionType.h"
#include "UVProjectionMappingEditor/Algorithms/IUVProjectionAlgorithm.h"
#include "Widgets/SWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "ShapePreviews/ShapePreviewBase.h"
#include "Framework/SlateDelegates.h"
#include "IDetailsView.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditor.h"
#include "RPRMeshData/RPRMeshDataContainer.h"

/*
 * Abstract class for UV Projection widgets
 */
class RPRPLUGINEDITOR_API SUVProjectionBase : public SCompoundWidget, public IUVProjectionSettingsWidget
{
public:
	SLATE_BEGIN_ARGS(SUVProjectionBase) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

	virtual FRPRMeshDataContainerPtr	GetMeshDatas() const override;
	virtual FRPRStaticMeshEditorPtr		GetRPRStaticMeshEditor() const;

	virtual TSharedRef<SWidget>		TakeWidget() override;
	virtual FOnProjectionApplied&	OnProjectionApplied() override;
	
	virtual void	SetRPRStaticMeshEditor(TWeakPtr<class FRPRStaticMeshEditor> RPRStaticMeshEditor) override;
	virtual void	OnUVProjectionDisplayed() override {}
	virtual void	OnUVProjectionHidden() override {}
	virtual void	OnSectionSelectionChanged() override {}

protected:
	
	void	InitUVProjection();
	void	InitWidget();

	/* Add component to the RPR Static Mesh Editor viewport */
	void	AddComponentToViewport(UActorComponent* InActorComponent, bool bSelectShape = true);

	/* Create a detail view widget for the shape preview */
	TSharedPtr<IDetailsView>	CreateShapePreviewDetailView(FName ViewIdentifier);

	/* Provide quick access to the algorithm for the UV mapping projection with the expected type */
	template<typename AlgorithmType>
	TSharedPtr<AlgorithmType>	GetAlgorithm() const;

	void	InitAlgorithm();
	void	StartAlgorithm();
	void	FinalizeAlgorithm();

	virtual bool	CanProject() const;
	virtual void	OnPreAlgorithmStart() {}
	
	virtual IUVProjectionAlgorithmPtr	CreateAlgorithm() const = 0;
	virtual TSharedRef<SWidget>			GetAlgorithmSettingsWidget() = 0;
	virtual void						OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess) = 0;
	virtual UShapePreviewBase*			GetShapePreview() = 0;
	virtual bool						RequiredManualApply() const;
	
private:

	FUVProjectionSettingsPtr	GetUVProjectionSettings() const;

	void	NotifyAlgorithmCompleted(IUVProjectionAlgorithmPtr AlgorithmInstance, bool bSuccess);
	void	AddShapePreviewToViewport();
	void	SubscribeToAlgorithmCompletion();
	FReply	OnApplyButtonClicked();
	EVisibility	GetApplyButtonVisibility() const;

protected:

	FRPRStaticMeshEditorWeakPtr	RPRStaticMeshEditorPtr;

private:

	IUVProjectionAlgorithmPtr	Algorithm;
	FOnProjectionApplied		OnProjectionAppliedDelegate;
	
	FUVProjectionSettingsPtr	UVProjectionSettings;

};


template<typename AlgorithmType>
TSharedPtr<AlgorithmType> SUVProjectionBase::GetAlgorithm() const
{
	return (StaticCastSharedPtr<AlgorithmType>(Algorithm));
}
