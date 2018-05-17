#include "RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialMapChannel1.h"
#include "SBox.h"

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialMapChannel1PropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialMapChannel1PropertiesLayout));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapChannel1PropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapChannel1, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapChannel1PropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapChannel1, Constant)));
}

TSharedPtr<SWidget> FRPRMaterialMapChannel1PropertiesLayout::GetConstantPropertyWidget() const
{
	return 
		SNew(SBox)
		.MinDesiredWidth(100.0f)
		[
			GetConstantPropertyHandle()->CreatePropertyValueWidget()
		];
}
