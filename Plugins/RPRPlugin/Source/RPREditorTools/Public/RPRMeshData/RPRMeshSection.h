#pragma once
#include "Delegate.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FBoolStateChange, bool)

class RPREDITORTOOLS_API FRPRMeshSection
{
public:
	
	FRPRMeshSection();
	
	bool	IsSelected() const;
	void	Select(bool bSelect = true);
	void	Deselect();

	void	Highlight(bool bEnable = true);
	bool	IsHighlighted() const;

	FBoolStateChange&	OnSelectionStateChanged() { return (OnSelectionStateChangedEvent); }
	FBoolStateChange&	OnHighlightStateChanged() { return (OnHighlightStateChangedEvent); }

private:

	bool	bIsSelected;
	bool	bIsHighlighted;

	FBoolStateChange	OnSelectionStateChangedEvent;
	FBoolStateChange	OnHighlightStateChangedEvent;

};