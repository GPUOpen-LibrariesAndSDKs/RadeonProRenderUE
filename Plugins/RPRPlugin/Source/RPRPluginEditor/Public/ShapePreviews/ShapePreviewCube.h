#pragma once

#include "ShapePreviewBase.h"
#include "CubeProjectionFace.h"
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

	UPROPERTY(EditAnywhere, Category = Preview)
	float FaceScale;

	UPROPERTY(EditAnywhere, Category = Preview)
	float FaceOffset;

	UPROPERTY(EditAnywhere, Category = Preview)
	float ArrowHeadSize;

	UPROPERTY(EditAnywhere, Category = Preview)
	float ArrowExtentSize;

private:

	FCubeProjectionFaces CubeProjectionFaces;

};