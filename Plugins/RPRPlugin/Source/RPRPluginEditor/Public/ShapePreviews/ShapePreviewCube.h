#pragma once

#include "ShapePreviewBase.h"
#include "ShapePreviewCube.generated.h"

UCLASS()
class UShapePreviewCube : public UShapePreviewBase
{
	GENERATED_BODY()
		
protected:

	UShapePreviewCube();

	virtual void DrawShapePreview() override;

public:

	UPROPERTY(EditAnywhere, Category = Preview)
	float Thickness;

};