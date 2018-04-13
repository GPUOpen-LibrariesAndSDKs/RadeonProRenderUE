#pragma once

#include "IUVProjectionSettingsWidget.h"
#include "Engine/StaticMesh.h"
#include "DeclarativeSyntaxSupport.h"
#include "UVProjectionType.h"
#include "IUVProjectionAlgorithm.h"
#include "SWidget.h"
#include "SCompoundWidget.h"
#include "ShapePreviewBase.h"
#include "SlateDelegates.h"
#include "IDetailsView.h"
#include "RPRStaticMeshEditor.h"
#include "RPRMeshDataContainer.h"

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
