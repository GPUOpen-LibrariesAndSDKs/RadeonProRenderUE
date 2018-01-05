#include "SUVProjectionBase.h"
#include "Engine/StaticMesh.h"
#include "AlgorithmFactory.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "SButton.h"

#define LOCTEXT_NAMESPACE "SUVProjectionBase"

void SUVProjectionBase::Construct(const FArguments& InArgs)
{
}

void SUVProjectionBase::SetStaticMesh(UStaticMesh* InStaticMesh)
{
	StaticMesh = InStaticMesh;
}

void SUVProjectionBase::SetRPRStaticMeshEditor(FRPRStaticMeshEditorWeakPtr InRPRStaticMeshEditor)
{
	RPRStaticMeshEditor = InRPRStaticMeshEditor;
}

TSharedRef<SWidget> SUVProjectionBase::TakeWidget()
{
	return (SharedThis(this));
}

UStaticMesh* SUVProjectionBase::GetStaticMesh() const
{
	return (StaticMesh);
}

FRPRStaticMeshEditorPtr SUVProjectionBase::GetRPRStaticMeshEditor() const
{
	return (RPRStaticMeshEditor.Pin());
}

void SUVProjectionBase::ConstructBase()
{
	Construct(FArguments());
}

void SUVProjectionBase::AddShapePreviewToViewport()
{
	AddComponentToViewport(GetShapePreview());
}

void SUVProjectionBase::AddComponentToViewport(UActorComponent* InActorComponent, bool bSelectShape /*= true*/)
{
	FRPRStaticMeshEditorPtr rprStaticMeshEditor = RPRStaticMeshEditor.Pin();
	if (rprStaticMeshEditor.IsValid())
	{
		rprStaticMeshEditor->AddComponentToViewport(InActorComponent, bSelectShape);
	}
}

void SUVProjectionBase::InitializeAlgorithm(EUVProjectionType ProjectionType)
{
	Algorithm = FAlgorithmFactory::CreateAlgorithm(StaticMesh, ProjectionType);
	SubscribeToAlgorithmCompletion();
}

void SUVProjectionBase::SubscribeToAlgorithmCompletion()
{
	Algorithm->OnAlgorithmCompleted().AddRaw(this, &SUVProjectionBase::OnAlgorithmCompleted);
}

void SUVProjectionBase::StartAlgorithm()
{
	if (Algorithm.IsValid())
	{
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
TSharedRef<class SWidget> SUVProjectionBase::CreateApplyButton(FOnClicked OnClicked) const
{
	return
		SNew(SButton)
		.HAlign(EHorizontalAlignment::HAlign_Center)
		.Text(LOCTEXT("ApplyButton", "Apply"))
		.OnClicked(OnClicked);
}

#undef LOCTEXT_NAMESPACE