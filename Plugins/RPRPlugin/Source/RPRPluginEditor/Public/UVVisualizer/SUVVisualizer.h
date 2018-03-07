#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "RawMesh.h"

class SUVVisualizer : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVVisualizer)
		: _StaticMesh()
	{}

		SLATE_ARGUMENT(TWeakObjectPtr<class UStaticMesh>, StaticMesh)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

	virtual int32	OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, 
						const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, 
						int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:

	void		PaintUVTriangle(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
							uint32 LayerId, const FSlateRect& UVBounds, const FLinearColor& Color,
							const FVector2D& PointA, const FVector2D& PointB, const FVector2D& PointC) const;

	FSlateRect	BuildUVBounds(const FVector2D& BoundsSize) const;
	FVector2D	ConvertLocalToAbsoluteUVPosition(const FSlateRect& UVBounds, const FVector2D& Point) const;

private:

	TWeakObjectPtr<UStaticMesh>	StaticMesh;
	FRawMesh RawMesh;

	TArray<FVector2D> ValidUVs;
	TArray<FVector2D> InvalidUVs;

};
