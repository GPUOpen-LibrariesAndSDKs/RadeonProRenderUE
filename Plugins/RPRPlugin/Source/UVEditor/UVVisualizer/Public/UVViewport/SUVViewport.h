#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "RawMesh.h"
#include "Engine/Texture2D.h"
#include "SlateBrush.h"
#include "UVViewportClient.h"
#include "SEditorViewport.h"

class SUVViewport : public SEditorViewport
{
public:

	SLATE_BEGIN_ARGS(SUVViewport) {}
	SLATE_END_ARGS()

	SUVViewport();

	void	Construct(const FArguments& InArgs);
	void	SetMesh(TWeakObjectPtr<class UStaticMesh> InStaticMesh);
	void	Refresh();

	void	SetUVChannelIndex(int32 ChannelIndex);
	void	SetBackground(UTexture2D* Image);
	void	ClearBackground();
	void	SetBackgroundOpacity(float Opacity);

	UStaticMesh*	GetStaticMesh() const;
	FRawMesh&		GetRawMesh();
	int32			GetUVChannel() const;

	virtual int32	OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
						const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements,
						int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	
protected:
	
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

private:

	void		PaintBackground(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, uint32 LayerId) const;

	void		PaintBackgroundImage(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
							uint32 LayerId, const FSlateRect& UVBounds) const;

	void		PaintUVs(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
							uint32 LayerId, const FSlateRect& UVBounds) const;

	void		PaintUVTriangle(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
						uint32 LayerId, const FSlateRect& UVBounds, const FLinearColor& Color,
						const FVector2D& PointA, const FVector2D& PointB, const FVector2D& PointC) const;

	void		PaintArrow(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
						uint32 LayerId, const FLinearColor& Color,
						const FVector2D& StartPoint, const FVector2D& EndPoint, float Thickness = 1.0f, float ArrowSize = 15.0f) const;

	void		PaintAxis(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
						uint32 LayerId, const FSlateRect& Bounds) const;

	FSlateRect	BuildUVBounds(const FVector2D& BoundsSize) const;
	FVector2D	ConvertLocalToAbsoluteUVPosition(const FSlateRect& UVBounds, const FVector2D& Point) const;

private:

	FUVViewportClientPtr ViewportClient;

	TWeakObjectPtr<UStaticMesh>	StaticMesh;
	FRawMesh RawMesh;
	int32 UVChannelIndex;

	FSlateBrush BackgroundBrush;
	float BackgroundOpacity;

};

typedef TSharedPtr<SUVViewport> SUVViewportPtr;