#include "SUVVisualizer.h"
#include "EditorStyleSet.h"
#include "DrawElements.h"
#include "SlateRectHelper/SlateRectHelper.h"
#include "UVUtility.h"
#include "StaticMeshHelper.h"
#include "RPRVectorTools.h"

SUVVisualizer::SUVVisualizer()
	: UVChannelIndex(0)
	, BackgroundOpacity(1.0f)
{}

void SUVVisualizer::Construct(const FArguments& InArgs)
{
	StaticMesh = InArgs._StaticMesh;
	Refresh();
}

void SUVVisualizer::Refresh()
{
	FStaticMeshHelper::LoadRawMeshFromStaticMesh(StaticMesh.Get(), RawMesh, 0);
}

void SUVVisualizer::SetUVChannelIndex(int32 ChannelIndex)
{
	int32 newUVChannelIndex = FMath::Min(ChannelIndex, StaticMesh->RenderData->LODResources[0].GetNumTexCoords() - 1);
	if (newUVChannelIndex != UVChannelIndex)
	{
		UVChannelIndex = newUVChannelIndex;
		Invalidate(EInvalidateWidget::Layout);
	}
}

void SUVVisualizer::SetBackground(UTexture2D* Image)
{
	BackgroundBrush.SetResourceObject(Image);
}

void SUVVisualizer::ClearBackground()
{
	SetBackground(nullptr);
}

void SUVVisualizer::SetBackgroundOpacity(float Opacity)
{
	BackgroundOpacity = Opacity;
}

int32 SUVVisualizer::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, 
					const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, 
					int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	OutDrawElements.PushClip(FSlateClippingZone(AllottedGeometry));

	FSlateRect uvBounds = BuildUVBounds(AllottedGeometry.GetLocalSize());
	uvBounds = uvBounds.ExtendBy(FMargin(-10));

	FPaintGeometry drawBox = 
		AllottedGeometry.ToPaintGeometry(
			uvBounds.GetTopLeft(), 
			FVector2D(FSlateRectHelper::GetWidth(uvBounds), FSlateRectHelper::GetHeight(uvBounds))
		);

	PaintBackground(OutDrawElements, drawBox, LayerId);
	++LayerId;

	if (BackgroundBrush.GetResourceObject() != nullptr)
	{
		PaintBackgroundImage(OutDrawElements, drawBox, LayerId, uvBounds);
		++LayerId;
	}
	
	PaintAxis(OutDrawElements, drawBox, LayerId, uvBounds);
	++LayerId;
	
	if (RawMesh.IsValid())
	{
		PaintUVs(OutDrawElements, drawBox, LayerId, uvBounds);
		++LayerId;
	}

	OutDrawElements.PopClip();

	return (SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled));
}

void SUVVisualizer::PaintBackground(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, uint32 LayerId) const
{
	const FSlateBrush* TimelineAreaBrush = FEditorStyle::GetBrush("Profiler.LineGraphArea");

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		TimelineAreaBrush,
		ESlateDrawEffect::None,
		FLinearColor(0, 0, 0, 0.4f)
	);
}

void SUVVisualizer::PaintBackgroundImage(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
											uint32 LayerId, const FSlateRect& UVBounds) const
{
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		&BackgroundBrush,
		ESlateDrawEffect::None,
		FLinearColor(1, 1, 1, BackgroundOpacity)
	);
}

void SUVVisualizer::PaintUVs(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
							uint32 LayerId, const FSlateRect& UVBounds) const
{
	const TArray<uint32>& triangles = RawMesh.WedgeIndices;
	for (int32 triIdx = 0; triIdx < triangles.Num(); triIdx += 3)
	{
		const FVector2D& uvA = RawMesh.WedgeTexCoords[UVChannelIndex][triIdx];
		const FVector2D& uvB = RawMesh.WedgeTexCoords[UVChannelIndex][triIdx + 1];
		const FVector2D& uvC = RawMesh.WedgeTexCoords[UVChannelIndex][triIdx + 2];

		if (FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
		{
			PaintUVTriangle(OutDrawElements, PaintGeometry, LayerId, UVBounds, FLinearColor::White, uvA, uvB, uvC);
		}
		else
		{
			PaintUVTriangle(OutDrawElements, PaintGeometry, LayerId, UVBounds, FLinearColor::Red, uvA, uvB, uvC);
		}
	}
}

void SUVVisualizer::PaintUVTriangle(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
									uint32 LayerId, const FSlateRect& UVBounds, const FLinearColor& Color,
									const FVector2D& PointA, const FVector2D& PointB, const FVector2D& PointC) const
{
	FVector2D absPointA = ConvertLocalToAbsoluteUVPosition(UVBounds, PointA);
	FVector2D absPointB = ConvertLocalToAbsoluteUVPosition(UVBounds, PointB);
	FVector2D absPointC = ConvertLocalToAbsoluteUVPosition(UVBounds, PointC);

	TArray<FVector2D> lines;
	lines.Reserve(2);

	// Draw [AB]
	lines.Add(absPointA);
	lines.Add(absPointB);
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		lines,
		ESlateDrawEffect::None,
		Color
	);

	// Draw [AC]
	// lines[0] = absPointA;
	lines[1] = absPointC;

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		lines,
		ESlateDrawEffect::None,
		Color
	);

	// Draw [BC]
	lines[0] = absPointB;
	lines[1] = absPointC;

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		lines,
		ESlateDrawEffect::None,
		Color
	);
}

void SUVVisualizer::PaintArrow(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
								uint32 LayerId, const FLinearColor& Color,
								const FVector2D& StartPoint, const FVector2D& EndPoint, 
								float Thickness, float ArrowSize) const
{
	const float arrowHeadAngle = FMath::DegreesToRadians(25);

	TArray<FVector2D> lines;
	lines.Reserve(2);

	lines.Add(StartPoint);
	lines.Add(EndPoint);

	// Draw arrow tail
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		lines,
		ESlateDrawEffect::None,
		Color,
		true,
		Thickness
	);

	FVector2D endToStartDirection = StartPoint - EndPoint;
	endToStartDirection.Normalize();

	lines[0] = EndPoint + FQuat2D(arrowHeadAngle).TransformVector(endToStartDirection) * ArrowSize;

	// Draw arrow head
	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		lines,
		ESlateDrawEffect::None,
		Color,
		true,
		Thickness
	);

	lines[0] = EndPoint + FQuat2D(-arrowHeadAngle).TransformVector(endToStartDirection) * ArrowSize;

	FSlateDrawElement::MakeLines(
		OutDrawElements,
		LayerId,
		PaintGeometry,
		lines,
		ESlateDrawEffect::None,
		Color,
		true,
		Thickness
	);
}

void SUVVisualizer::PaintAxis(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
								uint32 LayerId, const FSlateRect& Bounds) const
{
	const int32 arrowThickness = 2;
	const float width = FSlateRectHelper::GetWidth(Bounds);
	const float height = FSlateRectHelper::GetHeight(Bounds);

	PaintArrow(
		OutDrawElements,
		PaintGeometry,
		LayerId,
		FLinearColor(255, 100, 100),
		FVector2D(0, 0),
		FVector2D(width, 0),
		arrowThickness
	);

	PaintArrow(
		OutDrawElements,
		PaintGeometry,
		LayerId,
		FLinearColor(100, 255, 100),
		FVector2D(0, 0),
		FVector2D(0, height),
		arrowThickness
	);
}

FSlateRect SUVVisualizer::BuildUVBounds(const FVector2D& BoundsSize) const
{
	const float windowRectWidth = BoundsSize.X;
	const float windowRectHeight = BoundsSize.Y;

	const float minSize = FMath::Min(windowRectWidth, windowRectHeight);
	FSlateRect uvBounds = FSlateRectHelper::Create((windowRectWidth - minSize) / 2, (windowRectHeight - minSize) / 2, minSize, minSize);

	return (uvBounds);
}

FVector2D SUVVisualizer::ConvertLocalToAbsoluteUVPosition(const FSlateRect& UVBounds, const FVector2D& Point) const
{
	const float width = FSlateRectHelper::GetWidth(UVBounds);
	const float height = FSlateRectHelper::GetHeight(UVBounds);

	return (FVector2D(
		Point.X * width,
		Point.Y * height
	));
}

