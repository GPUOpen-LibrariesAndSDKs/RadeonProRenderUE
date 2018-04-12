#include "SUVProjectionTriPlanar.h"
#include "SBoxPanel.h"
#include "SScrollBox.h"
#include "SSpacer.h"
#include "IDetailsViewHelper.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "UVProjectionTriPlanarAlgo.h"

void SUVProjectionTriPlanar::Construct(const FArguments& InArgs)
{
	RPRStaticMeshEditorPtr = InArgs._RPRStaticMeshEditorPtr;

	InitTriPlanarSettings();
	InitUVProjection();
}

void SUVProjectionTriPlanar::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(Settings);
}

TSharedRef<SWidget> SUVProjectionTriPlanar::GetAlgorithmSettingsWidget()
{
	return SettingsDetailsView.ToSharedRef();
}

IUVProjectionAlgorithmPtr SUVProjectionTriPlanar::CreateAlgorithm() const
{
	return (MakeShareable(new FUVProjectionTriPlanarAlgo));
}

void SUVProjectionTriPlanar::OnAlgorithmCompleted(IUVProjectionAlgorithmPtr InAlgorithm, bool bIsSuccess)
{
	if (bIsSuccess)
	{
		FinalizeAlgorithm();
	}
}

UShapePreviewBase* SUVProjectionTriPlanar::GetShapePreview()
{
	return (nullptr);
}

void SUVProjectionTriPlanar::OnPreAlgorithmStart()
{
	UpdateAlgorithmSettings();
}

void SUVProjectionTriPlanar::InitTriPlanarSettings()
{
	Settings = NewObject<UTriPlanarSettings>();

	FPropertyEditorModule& propertyModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs detailsViewArgs(
		/*const bool InUpdateFromSelection =*/ false
		, /*const bool InLockable =*/ false
		, /*const bool InAllowSearch =*/ false
		, /*const ENameAreaSettings InNameAreaSettings =*/ FDetailsViewArgs::ENameAreaSettings::HideNameArea
		, /*const bool InHideSelectionTip =*/ false
		, /*FNotifyHook* InNotifyHook =*/ NULL
		, /*const bool InSearchInitialKeyFocus =*/ false
		, /*FName InViewIdentifier =*/ NAME_None
	);
	SettingsDetailsView = propertyModule.CreateDetailView(detailsViewArgs);
	SettingsDetailsView->SetObject(Settings, true);
}

void SUVProjectionTriPlanar::UpdateAlgorithmSettings()
{
	FUVProjectionTriPlanarAlgoPtr algo = GetAlgorithm<FUVProjectionTriPlanarAlgo>();
	
	FUVProjectionTriPlanarAlgo::FSettings algoSettings;
	{
		algoSettings.bApply = Settings->bUseTriPlanar;
		algoSettings.Angle = Settings->Angle;
		algoSettings.Scale = Settings->Scale;
	}

	algo->SetSettings(algoSettings);
}
