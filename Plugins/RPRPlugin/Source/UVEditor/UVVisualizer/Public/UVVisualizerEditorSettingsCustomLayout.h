#pragma once
#include "IDetailCustomization.h"
#include "SWidget.h"
#include "PropertyHandle.h"
#include "SharedPointer.h"
#include "Visibility.h"
#include "Delegate.h"

DECLARE_DELEGATE_RetVal(bool, FHasMeshChangesNotCommitted)

class FUVVisualizerEditorSettingsCustomLayout : public IDetailCustomization
{
public:

	struct FDelegates
	{
		FSimpleDelegate OnManualUpdate;
		FHasMeshChangesNotCommitted HasMeshChangesNotCommitted;
	};
	
public:

	FUVVisualizerEditorSettingsCustomLayout(const FDelegates& InDelegates);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	TSharedPtr<SWidget>	CreateManualUpdateWidget(TSharedPtr<IPropertyHandle> PropertyHandlePtr) const;

	EVisibility	GetManualUpdateWidgetVisibility(TSharedPtr<IPropertyHandle> PropertyHandle) const;
	bool		IsManualUpdateWidgetEnabled() const;
	FReply		OnManualUpdateButtonClicked();

private:

	FDelegates Delegates;

};
