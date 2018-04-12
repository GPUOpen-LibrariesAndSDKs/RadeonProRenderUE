#include "RPRMeshSection.h"

FRPRMeshSection::FRPRMeshSection()
	: bIsSelected(false)
	, bIsHighlighted(false)
{}

bool FRPRMeshSection::IsSelected() const
{
	return (bIsSelected);
}

void FRPRMeshSection::Select(bool bSelect)
{
	if (bSelect != bIsSelected)
	{
		bIsSelected = bSelect;
		OnSelectionStateChangedEvent.Broadcast(bIsSelected);
	}
}

void FRPRMeshSection::Deselect()
{
	Select(false);
}

void FRPRMeshSection::Highlight(bool bEnable /*= true*/)
{
	if (bEnable != bIsHighlighted)
	{
		bIsHighlighted = bEnable;
		OnSelectionStateChangedEvent.Broadcast(bIsHighlighted);
	}
}

bool FRPRMeshSection::IsHighlighted() const
{
	return (bIsHighlighted);
}
