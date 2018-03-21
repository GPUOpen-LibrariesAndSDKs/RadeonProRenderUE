#include "SUVViewport.h"
#include "EditorStyleSet.h"
#include "DrawElements.h"
#include "SlateRect.h"
#include "SlateRectHelper.h"
#include "UVUtility.h"
#include "StaticMeshHelper.h"
#include "RPRVectorTools.h"
#include "SUVViewportToolBar.h"
#include "UVViewportActions.h"
#include "SEditorViewport.h"
#include "UICommandList.h"
#include "UIAction.h"
#include "EditorViewportCommands.h"
#include "STransformViewportToolbar.h"
#include "SBorder.h"

SUVViewport::SUVViewport()
	: UVChannelIndex(INDEX_NONE)
	, BackgroundOpacity(1.0f)
{}

void SUVViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SUVViewport::SetMesh(TWeakObjectPtr<class UStaticMesh> InStaticMesh)
{
	StaticMesh = InStaticMesh;
	Refresh();
}

void SUVViewport::RebuildMeshFromRawMesh()
{
	if (StaticMesh.IsValid())
	{
		FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh.Get());
	}
}

void SUVViewport::Refresh()
{
	if (StaticMesh.IsValid())
	{
		FStaticMeshHelper::LoadRawMeshFromStaticMesh(StaticMesh.Get(), RawMesh);
		SetUVChannelIndex(FMath::Max(0, UVChannelIndex));
	}
	else
	{
		RawMesh.Empty();
		SetUVChannelIndex(INDEX_NONE);
	}
}

void SUVViewport::SetUVChannelIndex(int32 ChannelIndex)
{
	if (StaticMesh.IsValid())
	{
		const int32 numMaxTexCoords = StaticMesh->RenderData->LODResources[0].GetNumTexCoords();
		const int32 newUVChannelIndex = FMath::Min(ChannelIndex, numMaxTexCoords - 1);
		if (newUVChannelIndex != UVChannelIndex)
		{
			UVChannelIndex = newUVChannelIndex;

			ViewportClient->RegenerateUVCache();
			Invalidate();
		}
	}
	else
	{
		UVChannelIndex = INDEX_NONE;
	}
}

void SUVViewport::SetBackground(UTexture2D* Image)
{
	BackgroundBrush.SetResourceObject(Image);
}

void SUVViewport::ClearBackground()
{
	SetBackground(nullptr);
}

void SUVViewport::SetBackgroundOpacity(float Opacity)
{
	BackgroundOpacity = Opacity;
}

int32 SUVViewport::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, 
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
	
	if (StaticMesh.IsValid() && RawMesh.IsValid() && UVChannelIndex != INDEX_NONE)
	{
		PaintUVs(OutDrawElements, drawBox, LayerId, uvBounds);
		++LayerId;
	}

	OutDrawElements.PopClip();

	return (SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled));
}

void SUVViewport::PaintBackground(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, uint32 LayerId) const
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

void SUVViewport::PaintBackgroundImage(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
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

void SUVViewport::PaintUVs(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry,
							uint32 LayerId, const FSlateRect& UVBounds) const
{
	const TArray<uint32>& triangles = RawMesh.WedgeIndices;
	const TArray<FVector2D>& uv = RawMesh.WedgeTexCoords[UVChannelIndex];

	for (int32 triIdx = 0; triIdx < triangles.Num(); triIdx += 3)
	{
		if (!uv.IsValidIndex(triIdx))
		{
			break;
		}

		const FVector2D& uvA = uv[triIdx];
		const FVector2D& uvB = uv[triIdx + 1];
		const FVector2D& uvC = uv[triIdx + 2];

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

void SUVViewport::PaintUVTriangle(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
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

void SUVViewport::PaintArrow(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
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

void SUVViewport::PaintAxis(FSlateWindowElementList& OutDrawElements, const FPaintGeometry& PaintGeometry, 
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

FSlateRect SUVViewport::BuildUVBounds(const FVector2D& BoundsSize) const
{
	const float windowRectWidth = BoundsSize.X;
	const float windowRectHeight = BoundsSize.Y;

	const float minSize = FMath::Min(windowRectWidth, windowRectHeight);
	FSlateRect uvBounds = FSlateRectHelper::Create((windowRectWidth - minSize) / 2, (windowRectHeight - minSize) / 2, minSize, minSize);

	return (uvBounds);
}

FVector2D SUVViewport::ConvertLocalToAbsoluteUVPosition(const FSlateRect& UVBounds, const FVector2D& Point) const
{
	const float width = FSlateRectHelper::GetWidth(UVBounds);
	const float height = FSlateRectHelper::GetHeight(UVBounds);

	return (FVector2D(
		Point.X * width,
		Point.Y * height
	));
}

void SUVViewport::SelectAllUVs()
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->SelectAllUVs();
	}
}

TSharedRef<FEditorViewportClient> SUVViewport::MakeEditorViewportClient()
{
	ViewportClient = MakeShareable(new FUVViewportClient(StaticCastSharedRef<SUVViewport>(this->AsShared())));
	return (ViewportClient.ToSharedRef());
}

TSharedPtr<SWidget> SUVViewport::MakeViewportToolbar()
{
	return SNew(SUVViewportToolBar)
		.Viewport(SharedThis(this))
		.CommandList(CommandList);
}

void SUVViewport::BindCommands()
{
	const FUVViewportCommands& viewportActions = FUVViewportCommands::Get();
	TSharedRef<FUVViewportClient> clientRef = ViewportClient.ToSharedRef();

	CommandList->MapAction(
		viewportActions.SelectAllUV,
		FExecuteAction::CreateSP(this, &SUVViewport::SelectAllUVs)
	);

	CommandList->MapAction(
		FEditorViewportCommands::Get().TranslateMode,
		FExecuteAction::CreateSP(clientRef, &FEditorViewportClient::SetWidgetMode, FWidget::EWidgetMode::WM_Translate),
		FCanExecuteAction::CreateSP(clientRef, &FEditorViewportClient::CanSetWidgetMode, FWidget::WM_Translate),
		FIsActionChecked::CreateSP(this, &SUVViewport::IsWidgetModeActive, FWidget::WM_Translate)
		);

	CommandList->MapAction(
		FEditorViewportCommands::Get().RotateMode,
		FExecuteAction::CreateSP(clientRef, &FEditorViewportClient::SetWidgetMode, FWidget::EWidgetMode::WM_Rotate),
		FCanExecuteAction::CreateSP(clientRef, &FEditorViewportClient::CanSetWidgetMode, FWidget::WM_Rotate),
		FIsActionChecked::CreateSP(this, &SUVViewport::IsWidgetModeActive, FWidget::WM_Rotate)
	);

	CommandList->MapAction(
		FEditorViewportCommands::Get().ScaleMode,
		FExecuteAction::CreateSP(clientRef, &FEditorViewportClient::SetWidgetMode, FWidget::EWidgetMode::WM_Scale),
		FCanExecuteAction::CreateSP(clientRef, &FEditorViewportClient::CanSetWidgetMode, FWidget::WM_Scale),
		FIsActionChecked::CreateSP(this, &SUVViewport::IsWidgetModeActive, FWidget::WM_Scale)
	);

	DisableAction(FEditorViewportCommands::Get().TranslateRotateMode);
	DisableAction(FEditorViewportCommands::Get().TranslateRotate2DMode);
	DisableAction(FEditorViewportCommands::Get().SurfaceSnapping);
	DisableAction(FEditorViewportCommands::Get().CycleTransformGizmoCoordSystem);
}

void SUVViewport::DisableAction(TSharedPtr<FUICommandInfo> CommandInfo)
{
	static auto returnFalseFunc = []() { return (false);  };
	
	CommandList->MapAction(CommandInfo, 
		FExecuteAction(), 
		FCanExecuteAction(), 
		FIsActionChecked::CreateLambda(returnFalseFunc), 
		FIsActionButtonVisible::CreateLambda(returnFalseFunc)
	);
}

bool SUVViewport::IsWidgetModeActive(FWidget::EWidgetMode Mode) const
{
	return (SEditorViewport::IsWidgetModeActive(Mode));
}

UStaticMesh* SUVViewport::GetStaticMesh() const
{
	return (StaticMesh.Get());
}

FRawMesh& SUVViewport::GetRawMesh()
{
	return (RawMesh);
}

int32 SUVViewport::GetUVChannel() const
{
	return (UVChannelIndex);
}

const TArray<FVector2D>& SUVViewport::GetUV() const
{
	return (RawMesh.WedgeTexCoords[GetUVChannel()]);
}

TArray<FVector2D>& SUVViewport::GetUV()
{
	return (RawMesh.WedgeTexCoords[GetUVChannel()]);
}
