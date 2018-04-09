#include "SUVViewport.h"
#include "EditorStyleSet.h"
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
#include "RPRConstAway.h"

SUVViewport::SUVViewport()
	: UVChannelIndex(INDEX_NONE)
{}

void SUVViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SUVViewport::SetRPRMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas)
{
	if (RPRMeshDatas.IsValid())
	{
		UnsubscribeToPostRawMeshChange(*RPRMeshDatas.Pin());
	}

	RPRMeshDatas = InRPRMeshDatas;

	if (RPRMeshDatas.IsValid())
	{
		SubscribeToPostRawMeshChange(*RPRMeshDatas.Pin());
	}

	Refresh();
}

void SUVViewport::Refresh()
{
	FRPRMeshDataContainerPtr meshDataPtr = RPRMeshDatas.Pin();
	if (meshDataPtr.IsValid() && meshDataPtr->Num() > 0)
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

	FRPRMeshDataContainerPtr meshDataPtr = RPRMeshDatas.Pin();

	const int32 maxUVChannelIndex = meshDataPtr->GetMaxUVChannelUsedIndex();
	const int32 newUVChannelIndex = FMath::Min(ChannelIndex, maxUVChannelIndex);

	UVChannelIndex = newUVChannelIndex;
	ViewportClient->RefreshUV();
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

void SUVViewport::SubscribeToPostRawMeshChange(const FRPRMeshDataContainer& MeshDatas)
{
	PostRawMeshChangeDelegates.Reserve(MeshDatas.Num());
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		PostRawMeshChangeDelegates.Add(MeshDatas[i]->OnPostRawMeshChange.AddSP(this, &SUVViewport::Refresh));
	}
}

void SUVViewport::UnsubscribeToPostRawMeshChange(const FRPRMeshDataContainer& MeshDatas)
{
	check(MeshDatas.Num() == PostRawMeshChangeDelegates.Num());
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		MeshDatas[i]->OnPostRawMeshChange.Remove(PostRawMeshChangeDelegates[i]);
	}
	PostRawMeshChangeDelegates.Empty();
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
	TSharedRef<FUVViewportClient> clientRef = ViewportClient.ToSharedRef();
	
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

FRPRMeshDataContainerPtr SUVViewport::GetRPRMeshDatas() const
{
	return (RPRMeshDatas.Pin());
}

int32 SUVViewport::GetUVChannel() const
{
	return (UVChannelIndex);
}

TArray<FVector2D>& SUVViewport::GetUV(int32 MeshIndex)
{
	const SUVViewport* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetUV(MeshIndex)));
}

const TArray<FVector2D>& SUVViewport::GetUV(int32 MeshIndex) const
{
	check(RPRMeshDatas.IsValid());
	const FRPRMeshDataContainer& meshDatas = *RPRMeshDatas.Pin();
	return (meshDatas[MeshIndex]->GetRawMesh().WedgeTexCoords[GetUVChannel()]);
}