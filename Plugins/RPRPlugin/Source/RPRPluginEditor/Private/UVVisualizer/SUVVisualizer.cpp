#include "SUVVisualizer.h"
#include "EditorStyleSet.h"
#include "DrawElements.h"
#include "SlateRectHelper/SlateRectHelper.h"
#include "UVUtility.h"
#include "StaticMeshHelper.h"

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

int32 SUVVisualizer::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, 
					const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, 
					int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* TimelineAreaBrush = FEditorStyle::GetBrush("Profiler.LineGraphArea");

	FSlateRect uvBounds = BuildUVBounds(AllottedGeometry.GetLocalSize());

	FPaintGeometry drawBox = AllottedGeometry.ToPaintGeometry(uvBounds.GetTopLeft(), FVector2D(FSlateRectHelper::GetWidth(uvBounds), FSlateRectHelper::GetHeight(uvBounds)));

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		drawBox,
		TimelineAreaBrush,
		ESlateDrawEffect::None,
		FLinearColor(0, 0, 0, 0.4f)
	);
	++LayerId;

	if (RawMesh.IsValid())
	{
		const TArray<uint32>& triangles = RawMesh.WedgeIndices;
		for (int32 triIdx = 0; triIdx < triangles.Num(); triIdx += 3)
		{
			const FVector2D& uvA = RawMesh.WedgeTexCoords[UVChannelIndex][triIdx];
			const FVector2D& uvB = RawMesh.WedgeTexCoords[UVChannelIndex][triIdx + 1];
			const FVector2D& uvC = RawMesh.WedgeTexCoords[UVChannelIndex][triIdx + 2];

			if (FUVUtility::IsUVTriangleValid(uvA, uvB, uvC))
			{
				PaintUVTriangle(OutDrawElements, drawBox, LayerId, uvBounds, FLinearColor::White, uvA, uvB, uvC);
			}
			else
			{
				PaintUVTriangle(OutDrawElements, drawBox, LayerId, uvBounds, FLinearColor::Red, uvA, uvB, uvC);
			}
		}
		++LayerId;
	}

	return (SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled));
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
	return (FVector2D(
		Point.X * FSlateRectHelper::GetWidth(UVBounds),
		Point.Y * FSlateRectHelper::GetHeight(UVBounds)
	));
}

