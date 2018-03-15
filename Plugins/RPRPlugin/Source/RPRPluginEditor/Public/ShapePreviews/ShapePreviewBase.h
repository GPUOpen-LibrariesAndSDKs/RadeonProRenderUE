#pragma once

#include "Components/SceneComponent.h"
#include "SceneManagement.h"
#include "ShapePreviewBase.generated.h"

UCLASS(Abstract, HideCategories=(Rendering, Tags, ComponentReplication, Activation, Variable, Cooking, Physics, LOD, Collision, Mobility))
class RPRPLUGINEDITOR_API UShapePreviewBase : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UShapePreviewBase();

	const FColor&	GetShapeColor() const;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void Draw();

	virtual bool	CanBeTranslated() const;
	virtual bool	CanBeRotated() const;
	virtual bool	CanBeScaled() const;

protected:

	virtual void	DrawShapePreview() {};

	void DrawDebugAllAxis(const FTransform& InTransform, float AxisLength,
						float ArrowHeadSize, float ArrowThickness);

	void DrawDebugAxis(const FTransform& InTransform, EAxis::Type InAxis,
						const FColor& InColor, float InAxisLength,
						float InArrowHeadSize, float InArrowThickness);

private:
	
	void	BeginProxy(FPrimitiveDrawInterface* PDI);
	void	EndProxy(FPrimitiveDrawInterface* PDI);

private:

	UPROPERTY(EditAnywhere, Category = Preview)
	FColor	ShapeColor;

};