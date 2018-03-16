#include "SUVProjectionBase.h"
#include "AlgorithmFactory.h"
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

const TArray<UStaticMesh*>& SUVProjectionBase::GetStaticMeshes() const
{
	return (GetRPRStaticMeshEditor()->GetStaticMeshes());
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
	Algorithm->SetStaticMeshes(RPRStaticMeshEditorPtr.Pin()->GetStaticMeshes());
	SubscribeToAlgorithmCompletion();
}

void SUVProjectionBase::SubscribeToAlgorithmCompletion()
{
	Algorithm->OnAlgorithmCompleted().AddRaw(this, &SUVProjectionBase::NotifyAlgorithmCompleted);
}

FReply SUVProjectionBase::OnApplyButtonClicked()
{
	StartAlgorithm();
	return (FReply::Handled());
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
		Algorithm->SetGlobalUVProjectionSettings(UVProjectionSettings);
		OnPreAlgorithmStart();
		Algorithm->StartAlgorithm();
	}
}

void SUVProjectionBase::FinalizeAlgorithm()
{
	if (Algorithm.IsValid())
	{
		Algorithm->Finalize();
	}
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