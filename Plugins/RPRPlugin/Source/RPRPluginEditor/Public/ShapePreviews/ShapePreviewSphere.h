#pragma once

#include "Axis.h"
#include "ShapePreviewBase.h"
#include "ShapePreviewSphere.generated.h"

UCLASS()
class UShapePreviewSphere : public UShapePreviewBase
{
	GENERATED_BODY()

public:

	UShapePreviewSphere();

	virtual void DrawShapePreview() override;

	virtual bool	CanBeScaled() const;

private:

	void	DrawDebugAllAxis();
	void	DrawDebugAxis(EAxis::Type Axis, const FColor& Color);

public:

	UPROPERTY(EditAnywhere, Category = Preview)
	float	Radius;

	UPROPERTY(EditAnywhere, meta=(Tooltip="Does not affect the projection!"), Category = Preview)
	int32	NumSegments;

	UPROPERTY(EditAnywhere, Category = Preview)
	float	Thickness;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowHeadSize;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowThickness;

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Preview)
	float	ArrowExtentFactor;

};
