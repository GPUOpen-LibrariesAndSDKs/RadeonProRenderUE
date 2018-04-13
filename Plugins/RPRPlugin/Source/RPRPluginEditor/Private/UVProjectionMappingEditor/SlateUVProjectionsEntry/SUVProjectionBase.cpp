#include "SUVProjectionBase.h"
#include "PropertyEditorModule.h"
#include "SButton.h"
#include "SScrollBox.h"
#include "STextBlock.h"
#include "SSpacer.h"
#include "SGlobalUVProjectionSettings.h"

#define LOCTEXT_NAMESPACE "SUVProjectionBase"

void SUVProjectionBase::Construct(const FArguments& InArgs)
{
	InitUVProjection();
}

TSharedRef<SWidget> SUVProjectionBase::TakeWidget()
{
	return (SharedThis(this));
}

FOnProjectionApplied& SUVProjectionBase::OnProjectionApplied()
{
	return (OnProjectionAppliedDelegate);
}

void SUVProjectionBase::SetRPRStaticMeshEditor(FRPRStaticMeshEditorWeakPtr RPRStaticMeshEditor)
{
	RPRStaticMeshEditorPtr = RPRStaticMeshEditor;
}

FRPRMeshDataContainerPtr SUVProjectionBase::GetMeshDatas() const
{
	return (GetRPRStaticMeshEditor()->GetMeshDatas());
}

FRPRStaticMeshEditorPtr SUVProjectionBase::GetRPRStaticMeshEditor() const
{
	return (RPRStaticMeshEditorPtr.Pin());
}

void SUVProjectionBase::AddShapePreviewToViewport()
{
	AddComponentToViewport(GetShapePreview());
}

void SUVProjectionBase::InitUVProjection()
{
	UVProjectionSettings = MakeShareable(new FUVProjectionSettings);
	InitAlgorithm();
	InitWidget();
}

void SUVProjectionBase::InitWidget()
{
	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SGlobalUVProjectionSettings)
				.UVProjectionSettings(UVProjectionSettings)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				GetAlgorithmSettingsWidget()
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SSpacer)
			]
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SButton)
				.HAlign(EHorizontalAlignment::HAlign_Center)
				.Text(LOCTEXT("ProjectButton", "Project"))
				.OnClicked(this, &SUVProjectionBase::OnApplyButtonClicked)
				.IsEnabled(this, &SUVProjectionBase::CanProject)
				.Visibility(this, &SUVProjectionBase::GetApplyButtonVisibility)
			]
		];
}

void SUVProjectionBase::AddComponentToViewport(UActorComponent* InActorComponent, bool bSelectShape /*= true*/)
{
	FRPRStaticMeshEditorPtr rprStaticMeshEditor = RPRStaticMeshEditorPtr.Pin();
	if (rprStaticMeshEditor.IsValid())
	{
		rprStaticMeshEditor->AddComponentToViewport(InActorComponent, bSelectShape);
	}
}

void SUVProjectionBase::InitAlgorithm()
{
	Algorithm = CreateAlgorithm();
	SubscribeToAlgorithmCompletion();
}

void SUVProjectionBase::SubscribeToAlgorithmCompletion()
{
	if (Algorithm.IsValid())
	{
		Algorithm->OnAlgorithmCompleted().AddRaw(this, &SUVProjectionBase::NotifyAlgorithmCompleted);
	}
}

FReply SUVProjectionBase::OnApplyButtonClicked()
{
	StartAlgorithm();
	return (FReply::Handled());
}

EVisibility SUVProjectionBase::GetApplyButtonVisibility() const
{
	return (RequiredManualApply() ? EVisibility::Visible : EVisibility::Collapsed);
}

FUVProjectionSettingsPtr SUVProjectionBase::GetUVProjectionSettings() const
{
	return (UVProjectionSettings);
}

void SUVProjectionBase::NotifyAlgorithmCompleted(IUVProjectionAlgorithmPtr AlgorithmInstance, bool bSuccess)
{
	if (bSuccess)
	{
		OnAlgorithmCompleted(AlgorithmInstance, bSuccess);
		OnProjectionAppliedDelegate.ExecuteIfBound();
	}
}

void SUVProjectionBase::StartAlgorithm()
{
	if (Algorithm.IsValid())
	{
		FRPRMeshDataContainerPtr meshDataPtr = RPRStaticMeshEditorPtr.Pin()->GetSelectedMeshes();
		if (meshDataPtr.IsValid())
		{
			Algorithm->SetMeshDatas(*meshDataPtr);
			Algorithm->SetGlobalUVProjectionSettings(UVProjectionSettings);
			OnPreAlgorithmStart();
			Algorithm->StartAlgorithm();
		}
	}
}

void SUVProjectionBase::FinalizeAlgorithm()
{
	if (Algorithm.IsValid())
	{
		Algorithm->Finalize();
	}
}

bool SUVProjectionBase::CanProject() const
{
	FRPRMeshDataContainerPtr meshDataPtr = RPRStaticMeshEditorPtr.Pin()->GetSelectedMeshes();
	if (!meshDataPtr.IsValid())
	{
		return (false);
	}

	for (int32 meshIndex = 0; meshIndex < meshDataPtr->Num(); ++meshIndex)
	{
		FRPRMeshDataPtr meshData = meshDataPtr->Get(meshIndex);
		if (meshData->HasAtLeastOneSectionSelected())
		{
			return (true);
		}
	}

	return (false);
}

bool SUVProjectionBase::RequiredManualApply() const
{
	return (true);
}

TSharedPtr<IDetailsView> SUVProjectionBase::CreateShapePreviewDetailView(FName ViewIdentifier)
{
	FPropertyEditorModule& propertyEditorModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs detailsViewArgs(
		/*bUpdateFromSelection=*/ false,
		/*bLockable=*/ false,
		/*bAllowSearch=*/ false,
		FDetailsViewArgs::HideNameArea,
		/*bHideSelectionTip=*/ true,
		/*InNotifyHook=*/ nullptr,
		/*InSearchInitialKeyFocus=*/ false,
		/*InViewIdentifier=*/ ViewIdentifier);
	
	return (propertyEditorModule.CreateDetailView(detailsViewArgs));
}

#undef LOCTEXT_NAMESPACE