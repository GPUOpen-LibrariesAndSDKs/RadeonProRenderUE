#include "RPRMaterialBoolPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "SCheckBox.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialBoolPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialBoolPropertiesLayout()));
}

void FRPRMaterialBoolPropertiesLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &FRPRMaterialBoolPropertiesLayout::OnCheckStateChanged, PropertyHandle)
			.IsChecked(this, &FRPRMaterialBoolPropertiesLayout::IsChecked, PropertyHandle)
		];
}

void FRPRMaterialBoolPropertiesLayout::OnCheckStateChanged(ECheckBoxState CheckboxState, TSharedRef<IPropertyHandle> PropertyHandle)
{
	TArray<UObject*> objects;
	PropertyHandle->GetOuterObjects(objects);

	for (int32 i = 0; i < objects.Num(); ++i)
	{
		SetBoolValue(PropertyHandle, (CheckboxState == ECheckBoxState::Checked));
	}
}

ECheckBoxState FRPRMaterialBoolPropertiesLayout::IsChecked(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	TArray<UObject*> objects;
	PropertyHandle->GetOuterObjects(objects);

	bool bIsInitialized = false;
	bool bIsChecked = false;

	for (int32 i = 0; i < objects.Num(); ++i)
	{
		bool parameterBoolValue = GetBoolValue(PropertyHandle);
		if (!bIsInitialized)
		{
			bIsInitialized = true;
			bIsChecked = parameterBoolValue;
		}
		else if (parameterBoolValue != bIsChecked)
		{
			return (ECheckBoxState::Undetermined);
		}
	}

	return (bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
}

bool FRPRMaterialBoolPropertiesLayout::GetBoolValue(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	bool value;
	GetIsEnabledPropertyHandle(PropertyHandle)->GetValue(value);
	return (value);
}

void FRPRMaterialBoolPropertiesLayout::SetBoolValue(TSharedRef<IPropertyHandle> PropertyHandle, bool Value)
{
	GetIsEnabledPropertyHandle(PropertyHandle)->SetValue(Value);
}

TSharedPtr<IPropertyHandle> FRPRMaterialBoolPropertiesLayout::GetIsEnabledPropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	return (PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialBool, bIsEnabled)));
}
