/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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

	GetUVUpdateMethodDelegate = InArgs._GetUVUpdateMethod;
}

void SUVViewport::SetRPRMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas)
{
	if (RPRMeshDatas.IsValid())
	{
		UnsubscribeToPostRawMeshChange(*RPRMeshDatas.Pin());
		UnsubscribeToPostStaticMeshChange(*RPRMeshDatas.Pin());
	}

	RPRMeshDatas = InRPRMeshDatas;

	if (RPRMeshDatas.IsValid())
	{
		SubscribeToPostRawMeshChange(*RPRMeshDatas.Pin());
		SubscribeToPostStaticMeshChange(*RPRMeshDatas.Pin());
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
	ViewportClient->RedrawRequested(nullptr);
	Invalidate();
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

void SUVViewport::SubscribeToPostStaticMeshChange(const FRPRMeshDataContainer& MeshDatas)
{
	PostStaticMeshChangeDelegates.Reserve(MeshDatas.Num());
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		PostStaticMeshChangeDelegates.Add(MeshDatas[i]->OnPostStaticMeshChange.AddSP(this, &SUVViewport::ClearUVTransform));
	}
}

void SUVViewport::UnsubscribeToPostStaticMeshChange(const FRPRMeshDataContainer& MeshDatas)
{
	check(MeshDatas.Num() == PostStaticMeshChangeDelegates.Num());
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		MeshDatas[i]->OnPostStaticMeshChange.Remove(PostStaticMeshChangeDelegates[i]);
	}
	PostStaticMeshChangeDelegates.Empty();
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

void SUVViewport::ClearUVTransform()
{
	ViewportClient->ClearUVTransform();
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

EUVUpdateMethod SUVViewport::GetUVUpdateMethod() const
{
	return (GetUVUpdateMethodDelegate.Execute());
}

void SUVViewport::ApplyUVTransform()
{
	ViewportClient->ApplyUVTransform();
	ViewportClient->ClearUVTransform();
}
