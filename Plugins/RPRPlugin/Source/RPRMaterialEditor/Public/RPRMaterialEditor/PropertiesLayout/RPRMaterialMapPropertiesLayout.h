#pragma once
#include "IPropertyTypeCustomization.h"

class FRPRMaterialMapPropertiesLayout : public IPropertyTypeCustomization
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

protected:

	TSharedPtr<IPropertyHandle> GetTexturePropertyHandle() const;


protected:

	TSharedPtr<IPropertyHandle>		CurrentPropertyHandle;

};