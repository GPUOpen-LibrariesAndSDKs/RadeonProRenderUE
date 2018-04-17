#include "RPRStaticMeshEditorModeManager.h"
#include "RPRConstAway.h"

TSharedPtr<FRPRStaticMeshEditorModeManager> FRPRStaticMeshEditorModeManager::Instance;

FRPRStaticMeshEditorModeManager& FRPRStaticMeshEditorModeManager::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FRPRStaticMeshEditorModeManager);
	}

	return (*Instance.Get());
}

void FRPRStaticMeshEditorModeManager::RegisterMode(TSharedPtr<FEdMode> Mode)
{
	Modes.Add(Mode->GetID(), Mode);
}

TSharedPtr<FEdMode> FRPRStaticMeshEditorModeManager::FindMode(FEditorModeID ModeID)
{
	const FRPRStaticMeshEditorModeManager* thisConst = this;
	return (RPR::ConstRefAway(thisConst->FindMode(ModeID)));
}

const TSharedPtr<FEdMode> FRPRStaticMeshEditorModeManager::FindMode(FEditorModeID ModeID) const
{
	const TSharedPtr<FEdMode>* foundMode = Modes.Find(ModeID);
	if (foundMode != nullptr && foundMode->IsValid())
	{
		return (*foundMode);
	}

	return (nullptr);
}
