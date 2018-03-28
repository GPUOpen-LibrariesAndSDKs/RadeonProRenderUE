#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "RawMesh.h"
#include "Engine/Texture2D.h"
#include "SlateBrush.h"
#include "UVViewportClient.h"
#include "SEditorViewport.h"
#include "RPRMeshData.h"

class SUVViewport : public SEditorViewport
{
public:

	SLATE_BEGIN_ARGS(SUVViewport) {}
	SLATE_END_ARGS()

	SUVViewport();

	void	Construct(const FArguments& InArgs);
	void	SetRPRMeshData(TWeakPtr<FRPRMeshData> InRPRMeshData);
	void	Refresh();

	void	SetUVChannelIndex(int32 ChannelIndex);
	void	SetBackground(UTexture2D* Image);
	void	ClearBackground();
	void	SetBackgroundOpacity(float Opacity);

	TWeakPtr<FRPRMeshData>		GetRPRMeshData() const;
	int32						GetUVChannel() const;
	TArray<FVector2D>&			GetUV();
	const TArray<FVector2D>&	GetUV() const;
	
protected:
	
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual TSharedPtr<SWidget> MakeViewportToolbar() override;
	
	virtual void BindCommands() override;
	void DisableAction(TSharedPtr<FUICommandInfo> CommandInfo);
	virtual bool IsWidgetModeActive(FWidget::EWidgetMode Mode) const override;

private:

	void		SelectAllUVs();

private:

	FUVViewportClientPtr ViewportClient;

	TWeakPtr<FRPRMeshData> RPRMeshData;
	int32 UVChannelIndex;

};

typedef TSharedPtr<SUVViewport> SUVViewportPtr;