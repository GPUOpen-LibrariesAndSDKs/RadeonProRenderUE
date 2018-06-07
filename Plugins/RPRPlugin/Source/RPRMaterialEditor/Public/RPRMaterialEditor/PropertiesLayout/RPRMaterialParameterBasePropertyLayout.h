#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "Visibility.h"

class FRPRMaterialParameterBasePropertyLayout : public IPropertyTypeCustomization
{
public:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}
	
protected:

	virtual TSharedRef<SWidget> GetPropertyValueRowWidget() = 0;

private:

	EVisibility		GetPropertyVisibility() const;
	EVisibility		GetSupportIconVisibility() const;
	EVisibility		GetInfoIconVisibility() const;
	FText			GetInfoText() const;

	TSharedPtr<IPropertyHandle>		GetInfoPropertyHandle() const;

protected:

	TSharedPtr<IPropertyHandle>		CurrentPropertyHandle;

};
