#include "SUVProjectionPlanar.h"
#include "Engine/StaticMesh.h"
#include "SBoxPanel.h"
#include "SButton.h"

#define LOCTEXT_NAMESPACE "SUVProjectionPlanar"

void SUVProjectionPlanar::Construct(const FArguments& InArgs)
{
	ConstructBase();
	
	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.VAlign(EVerticalAlignment::VAlign_Top)
			[
				SNew(SButton)
				.Text(LOCTEXT("ApplyButton", "Apply"))
				.OnClicked(this, &SUVProjectionPlanar::Apply)
			]
		];
}

FReply SUVProjectionPlanar::Apply()
{
	StartAlgorithm();
	return (FReply::Handled());
}

void SUVProjectionPlanar::FinalizeCreation()
{
	InitializeAlgorithm(EUVProjectionType::Planar);
}

void SUVProjectionPlanar::OnAlgorithmCompleted(IUVProjectionAlgorithm* Algorithm, bool bIsSuccess)
{
	if (bIsSuccess)
	{
		FinalizeAlgorithm();
	}
}

#undef LOCTEXT_NAMESPACE