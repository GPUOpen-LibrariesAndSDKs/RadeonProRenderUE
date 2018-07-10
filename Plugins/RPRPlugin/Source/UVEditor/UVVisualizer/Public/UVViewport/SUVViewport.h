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
#pragma once

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "RawMesh.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "UVViewport/UVViewportClient.h"
#include "SEditorViewport.h"
#include "RPRMeshData/RPRMeshDataContainer.h"
#include "UVVisualizerEditorSettings.h"

DECLARE_DELEGATE_RetVal(EUVUpdateMethod, FGetUVUpdateMethod)

class SUVViewport : public SEditorViewport
{
public:

	SLATE_BEGIN_ARGS(SUVViewport) {}
		SLATE_EVENT(FGetUVUpdateMethod, GetUVUpdateMethod)
	SLATE_END_ARGS()

	SUVViewport();

	void	Construct(const FArguments& InArgs);
	void	SetRPRMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas);
	void	Refresh();
	
	void	SetUVChannelIndex(int32 ChannelIndex);
	void	SetBackground(UTexture2D* Image);
	void	ClearBackground();
	void	SetBackgroundOpacity(float Opacity);

	FRPRMeshDataContainerPtr	GetRPRMeshDatas() const;
	int32						GetUVChannel() const;
	TArray<FVector2D>&			GetUV(int32 MeshIndex);
	const TArray<FVector2D>&	GetUV(int32 MeshIndex) const;
	EUVUpdateMethod				GetUVUpdateMethod() const;

	void	ApplyUVTransform();
	
protected:
	
	void SubscribeToPostRawMeshChange(const FRPRMeshDataContainer& MeshDatas);
	void UnsubscribeToPostRawMeshChange(const FRPRMeshDataContainer& MeshDatas);
	void SubscribeToPostStaticMeshChange(const FRPRMeshDataContainer& MeshDatas);
	void UnsubscribeToPostStaticMeshChange(const FRPRMeshDataContainer& MeshDatas);
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	
	virtual void BindCommands() override;
	void DisableAction(TSharedPtr<FUICommandInfo> CommandInfo);
	virtual bool IsWidgetModeActive(FWidget::EWidgetMode Mode) const override;

	void	ClearUVTransform();
	
private:

	FUVViewportClientPtr ViewportClient;

	FRPRMeshDataContainerWkPtr RPRMeshDatas;
	int32 UVChannelIndex;

	FGetUVUpdateMethod GetUVUpdateMethodDelegate;

	TArray<FDelegateHandle>	PostRawMeshChangeDelegates;
	TArray<FDelegateHandle>	PostStaticMeshChangeDelegates;
};

typedef TSharedPtr<SUVViewport> SUVViewportPtr;
