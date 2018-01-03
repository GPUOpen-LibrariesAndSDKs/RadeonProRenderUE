#pragma once

#include "Components/SceneComponent.h"
#include "SceneManagement.h"
#include "ShapePreviewBase.generated.h"

UCLASS(Abstract, HideCategories=(Rendering, Tags, ComponentReplication, Activation, Variable, Cooking, Physics, LOD, Collision, Mobility))
class UShapePreviewBase : public USceneComponent
{
	GENERATED_BODY()

public:
	
	UShapePreviewBase();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual void Draw();

	virtual bool	CanBeTranslated() const;
	virtual bool	CanBeRotated() const;
	virtual bool	CanBeScaled() const;

protected:

	virtual void	DrawShapePreview() {};

private:
	
	void	BeginProxy(FPrimitiveDrawInterface* PDI);
	void	EndProxy(FPrimitiveDrawInterface* PDI);

};