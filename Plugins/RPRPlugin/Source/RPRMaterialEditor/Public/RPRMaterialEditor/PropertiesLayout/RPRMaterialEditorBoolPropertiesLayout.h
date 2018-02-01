#pragma once
#include "IPropertyTypeCustomization.h"
#include "SlateTypes.h"
#include "RPRMaterialBool.h"

class FRPRMaterialEditorBoolPropertiesLayout : public IPropertyTypeCustomization
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

private:

	void			OnCheckStateChanged(ECheckBoxState CheckboxState, TSharedRef<IPropertyHandle> PropertyHandle);
	ECheckBoxState	IsChecked(TSharedRef<IPropertyHandle> PropertyHandle) const;

	bool			GetBoolValue(TSharedRef<IPropertyHandle> PropertyHandle) const;
	void			SetBoolValue(TSharedRef<IPropertyHandle> PropertyHandle, bool Value);

	TSharedPtr<IPropertyHandle>		GetIsEnabledPropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const;

};
