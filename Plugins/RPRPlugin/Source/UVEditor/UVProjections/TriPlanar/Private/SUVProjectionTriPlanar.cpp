#include "SUVProjectionTriPlanar.h"
#include "SBoxPanel.h"
#include "SScrollBox.h"
#include "SSpacer.h"
#include "IDetailsViewHelper.h"
#include "ModuleManager.h"
#include "PropertyEditorModule.h"
#include "UVProjectionTriPlanarAlgo.h"
#include "STextBlock.h"
#include "SScaleBox.h"
#include "SImage.h"
#include "SBox.h"

#define LOCTEXT_NAMESPACE "SUVProjectionTriPlanar"

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
	return 
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		.Padding(10.0f)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				SNew(SScaleBox)
				.HAlign(HAlign_Center)
				.Content()
				[
					SNew(SImage)
					.Image(FEditorStyle::GetBrush("SettingsEditor.WarningIcon"))
				]
			]
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Center)
			.FillWidth(1.0f)
			[
				SNew(SBox)
				.ToolTipText(LOCTEXT("TriPlanarWorkaround", "You can manually duplicate the materials and assign to the different sections, so everything will not be affected by the TriPlanar."))
				[
					SNew(STextBlock)
					.Margin(FMargin(5.0f, 0.0f))
					.AutoWrapText(true)
					.Text(LOCTEXT("TriPlanarWarning", "The TriPlanar modifier affects the selected materials (not UV)."))
				]
			]
		]
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			SettingsDetailsView.ToSharedRef()
		]
	;
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

bool SUVProjectionTriPlanar::RequiredManualApply() const
{
	return (false);
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
		, /*FNotifyHook* InNotifyHook =*/ this
		, /*const bool InSearchInitialKeyFocus =*/ false
		, /*FName InViewIdentifier =*/ NAME_None
	);
	SettingsDetailsView = propertyModule.CreateDetailView(detailsViewArgs);
	SettingsDetailsView->SetObject(Settings, true);
}

void SUVProjectionTriPlanar::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FEditPropertyChain* PropertyThatChanged)
{
	StartAlgorithm();
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

#undef LOCTEXT_NAMESPACE