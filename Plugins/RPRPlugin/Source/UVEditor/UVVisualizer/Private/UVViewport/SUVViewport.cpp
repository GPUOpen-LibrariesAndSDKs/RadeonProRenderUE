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
{}

void SUVViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SUVViewport::SetRPRMeshData(TWeakPtr<FRPRMeshData> InRPRMeshData)
{
	if (InRPRMeshData != RPRMeshData)
	{
		RPRMeshData = InRPRMeshData;
		if (RPRMeshData.IsValid())
		{
			RPRMeshData.Pin()->OnPostRawMeshChange.AddSP(this, &SUVViewport::Refresh);
		}
		Refresh();
	}
}

void SUVViewport::Refresh()
{
	if (RPRMeshData.IsValid())
	{
		SetUVChannelIndex(FMath::Max(0, UVChannelIndex));
	}
	else
	{
		SetUVChannelIndex(INDEX_NONE);
	}
}

void SUVViewport::SetUVChannelIndex(int32 ChannelIndex)
{
	bool bIsMeshValid = false;

	if (RPRMeshData.IsValid())
	{
		UStaticMesh* staticMesh = RPRMeshData.Pin()->GetStaticMesh();
		if (staticMesh != nullptr && staticMesh->HasValidRenderData())
		{
			bIsMeshValid = true;
			const int32 numMaxTexCoords = staticMesh->RenderData->LODResources[0].GetNumTexCoords();
			const int32 newUVChannelIndex = FMath::Min(ChannelIndex, numMaxTexCoords - 1);
			if (newUVChannelIndex != UVChannelIndex)
			{
				UVChannelIndex = newUVChannelIndex;
			}

			ViewportClient->RegenerateUVCache();
		}
	}
	
	if (!bIsMeshValid)
	{
		UVChannelIndex = INDEX_NONE;
	}

	Invalidate();
}

void SUVViewport::SetBackground(UTexture2D* Image)
{
	ViewportClient->SetBackgroundImage(Image);
}

void SUVViewport::ClearBackground()
{
	ViewportClient->SetBackgroundImage(nullptr);
}

void SUVViewport::SetBackgroundOpacity(float Opacity)
{
	ViewportClient->SetBackgroundOpacity(Opacity);
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

TWeakPtr<FRPRMeshData> SUVViewport::GetRPRMeshData() const
{
	return (RPRMeshData);
}

int32 SUVViewport::GetUVChannel() const
{
	return (UVChannelIndex);
}

TArray<FVector2D>& SUVViewport::GetUV()
{
	check(RPRMeshData.IsValid());
	return (RPRMeshData.Pin()->GetRawMesh().WedgeTexCoords[GetUVChannel()]);
}

const TArray<FVector2D>& SUVViewport::GetUV() const
{
	check(RPRMeshData.IsValid());
	return (RPRMeshData.Pin()->GetRawMesh().WedgeTexCoords[GetUVChannel()]);
}