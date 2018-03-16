#pragma once

#include "ShapePreviewBase.h"
#include "ShapePreviewCylinder.generated.h"

UCLASS()
class UShapePreviewCylinder : public UShapePreviewBase
{
	GENERATED_BODY()

public:

	UShapePreviewCylinder();

	virtual void	DrawShapePreview() override;
	virtual bool	CanBeScaled() const override;

public:

	UPROPERTY(EditAnywhere, meta=(Tooltip="Does not affect the calcul"), Category = Preview)
	float	Radius;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Does affect the calcul"), Category = Preview)
	float	Height;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Does not affect the calcul"), Category = Preview)
	float	Thickness;

	UPROPERTY(EditAnywhere, meta = (Tooltip = "Does not affect the calcul"), Category = Preview)
	int32	NumSegments;


	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowHeadSize;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowThickness;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowExtentFactor;
};
