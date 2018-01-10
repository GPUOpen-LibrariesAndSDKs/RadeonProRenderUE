#pragma once

#include "ShapePreviewBase.h"
#include "ShapePreviewPlane.generated.h"

UCLASS()
class UShapePreviewPlane : public UShapePreviewBase
{
	GENERATED_BODY()

public:

	UShapePreviewPlane();
	
	void	SetThickness(float InThickness);
	void	SetInitialPlaneDatas(const struct FPlane& InPlane);

	virtual bool CanBeScaled() const override;

protected:

	virtual void DrawShapePreview() override;

private:

	struct FPlane	InitialPlane;

	UPROPERTY(EditAnywhere, meta=(UIMin="0.1", UIMax="2"), Category = Preview)
	float	Thickness;

	UPROPERTY(EditAnywhere, Category = Preview)
	float	PlaneScale;

	UPROPERTY(EditAnywhere, Category = Preview)
	float	ArrowHeadSize;

	UPROPERTY(EditAnywhere, Category = Preview)
	float	ArrowThickness;
};