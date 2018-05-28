#include "RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialConstantOrMapChannel1.h"
#include "SBox.h"

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialConstantOrMapChannel1PropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialConstantOrMapChannel1PropertiesLayout));
}

TSharedPtr<IPropertyHandle> FRPRMaterialConstantOrMapChannel1PropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialConstantOrMapChannel1, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialConstantOrMapChannel1PropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialConstantOrMapChannel1, Constant)));
}

TSharedPtr<SWidget> FRPRMaterialConstantOrMapChannel1PropertiesLayout::GetConstantPropertyWidget() const
{
	return 
		SNew(SBox)
		.MinDesiredWidth(100.0f)
		[
			GetConstantPropertyHandle()->CreatePropertyValueWidget()
		];
}
