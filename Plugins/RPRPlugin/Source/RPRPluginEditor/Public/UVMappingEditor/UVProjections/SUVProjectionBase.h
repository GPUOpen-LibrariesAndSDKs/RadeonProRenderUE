#pragma once

#include "IUVProjection.h"
#include "DeclarativeSyntaxSupport.h"
#include "UVProjectionType.h"
#include "IUVProjectionAlgorithm.h"
#include "SCompoundWidget.h"
#include "ShapePreviewBase.h"
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

	virtual void				SetStaticMesh(class UStaticMesh* InStaticMesh) override;
	virtual void				SetRPRStaticMeshEditor(FRPRStaticMeshEditorWeakPtr InRPRStaticMeshEditor) override;
	virtual UStaticMesh*		GetStaticMesh() const override;
	virtual TSharedRef<SWidget> TakeWidget() override;

protected:
	
	void	ConstructBase();

	void	AddComponentToViewport(UActorComponent* InActorComponent, bool bSelectShape = true);

	void	InitializeAlgorithm(EUVProjectionType ProjectionType);
	void	StartAlgorithm();
	void	FinalizeAlgorithm();

	virtual void				InitializePostSetRPRStaticMeshEditor();

	virtual void				OnAlgorithmCompleted(IUVProjectionAlgorithm* Algorithm, bool bIsSuccess) = 0;
	virtual UShapePreviewBase*	GetShapePreview() = 0;

	template<typename AlgorithmType>
	TSharedPtr<AlgorithmType>	GetAlgorithm() const;

private:

	void	AddShapePreviewToViewport();
	void	SubscribeToAlgorithmCompletion();

private:

	IUVProjectionAlgorithmPtr	Algorithm;
	UStaticMesh*				StaticMesh;
	FRPRStaticMeshEditorWeakPtr	RPRStaticMeshEditor;

};


template<typename AlgorithmType>
TSharedPtr<AlgorithmType> SUVProjectionBase::GetAlgorithm() const
{
	return (StaticCastSharedPtr<AlgorithmType>(Algorithm));
}
