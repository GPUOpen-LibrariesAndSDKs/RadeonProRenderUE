#pragma once

#include "IUVProjection.h"
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

/*
 * Abstract class for UV Projection widgets
 */
class SUVProjectionBase : public SCompoundWidget, public IUVProjection
{
public:
	SLATE_BEGIN_ARGS(SUVProjectionBase) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

	virtual void	SetStaticMesh(UStaticMesh* InStaticMesh) override;
	virtual void	SetRPRStaticMeshEditor(FRPRStaticMeshEditorWeakPtr InRPRStaticMeshEditor) override;

	virtual UStaticMesh*			GetStaticMesh() const override;
	virtual FRPRStaticMeshEditorPtr GetRPRStaticMeshEditor() const;

	virtual TSharedRef<SWidget>		TakeWidget() override;
	virtual FOnProjectionApplied&	OnProjectionApplied() override;

protected:
	
	void	ConstructBase();

	/* Add component to the RPR Static Mesh Editor viewport */
	void	AddComponentToViewport(UActorComponent* InActorComponent, bool bSelectShape = true);

	/* Create a detail view widget for the shape preview */
	TSharedPtr<IDetailsView>	CreateShapePreviewDetailView(FName ViewIdentifier);
	
	TSharedRef<SWidget>			CreateProjectButton(FOnClicked OnClicked) const;

	/* Provide quick access to the algorithm for the UV mapping projection with the expected type */
	template<typename AlgorithmType>
	TSharedPtr<AlgorithmType>	GetAlgorithm() const;

	void	InitializeAlgorithm(EUVProjectionType ProjectionType);
	void	StartAlgorithm();
	void	FinalizeAlgorithm();

	virtual void				OnAlgorithmCompleted(IUVProjectionAlgorithm* InAlgorithm, bool bIsSuccess) = 0;
	virtual UShapePreviewBase*	GetShapePreview() = 0;

private:

	void	NotifyAlgorithmCompleted(IUVProjectionAlgorithm* AlgorithmInstance, bool bSuccess);
	void	AddShapePreviewToViewport();
	void	SubscribeToAlgorithmCompletion();

private:

	IUVProjectionAlgorithmPtr	Algorithm;
	UStaticMesh*				StaticMesh;
	FRPRStaticMeshEditorWeakPtr	RPRStaticMeshEditor;
	FOnProjectionApplied		OnProjectionAppliedDelegate;
};


template<typename AlgorithmType>
TSharedPtr<AlgorithmType> SUVProjectionBase::GetAlgorithm() const
{
	return (StaticCastSharedPtr<AlgorithmType>(Algorithm));
}
