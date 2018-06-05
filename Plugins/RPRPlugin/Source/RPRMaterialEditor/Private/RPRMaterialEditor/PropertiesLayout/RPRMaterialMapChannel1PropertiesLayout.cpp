#include "RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialCoMChannel1.h"
#include "SBox.h"

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialCoMChannel1PropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialCoMChannel1PropertiesLayout));
}

TSharedPtr<IPropertyHandle> FRPRMaterialCoMChannel1PropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialCoMChannel1, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialCoMChannel1, Constant)));
}

TSharedPtr<SWidget> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantPropertyWidget() const
{
	return 
		SNew(SBox)
		.MinDesiredWidth(100.0f)
		[
			GetConstantPropertyHandle()->CreatePropertyValueWidget()
		];
}
