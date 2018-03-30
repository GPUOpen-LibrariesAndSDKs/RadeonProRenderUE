#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "RawMesh.h"
#include "Engine/Texture2D.h"
#include "SlateBrush.h"
#include "UVViewportClient.h"
#include "SEditorViewport.h"
#include "RPRMeshDataContainer.h"

class SUVViewport : public SEditorViewport
{
public:

	SLATE_BEGIN_ARGS(SUVViewport) {}
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
	
protected:
	
	void SubscribeToPostRawMeshChange(const FRPRMeshDataContainer& MeshDatas);
	void UnsubscribeToPostRawMeshChange(const FRPRMeshDataContainer& MeshDatas);
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	
	virtual void BindCommands() override;
	void DisableAction(TSharedPtr<FUICommandInfo> CommandInfo);
	virtual bool IsWidgetModeActive(FWidget::EWidgetMode Mode) const override;
	
private:

	FUVViewportClientPtr ViewportClient;

	FRPRMeshDataContainerWkPtr RPRMeshDatas;
	int32 UVChannelIndex;

	TArray<FDelegateHandle>	PostRawMeshChangeDelegates;
};

typedef TSharedPtr<SUVViewport> SUVViewportPtr;