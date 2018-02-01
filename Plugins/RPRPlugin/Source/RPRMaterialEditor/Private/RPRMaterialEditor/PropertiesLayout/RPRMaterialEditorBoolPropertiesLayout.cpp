#include "RPRMaterialEditorBoolPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "SCheckBox.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialEditorBoolPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialEditorBoolPropertiesLayout()));
}

void FRPRMaterialEditorBoolPropertiesLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		[
			SNew(SCheckBox)
			.OnCheckStateChanged(this, &FRPRMaterialEditorBoolPropertiesLayout::OnCheckStateChanged, PropertyHandle)
			.IsChecked(this, &FRPRMaterialEditorBoolPropertiesLayout::IsChecked, PropertyHandle)
		];
}

void FRPRMaterialEditorBoolPropertiesLayout::OnCheckStateChanged(ECheckBoxState CheckboxState, TSharedRef<IPropertyHandle> PropertyHandle)
{
	TArray<UObject*> objects;
	PropertyHandle->GetOuterObjects(objects);

	for (int32 i = 0; i < objects.Num(); ++i)
	{
		SetBoolValue(PropertyHandle, (CheckboxState == ECheckBoxState::Checked));
	}
}

ECheckBoxState FRPRMaterialEditorBoolPropertiesLayout::IsChecked(TSharedRef<IPropertyHandle> PropertyHandle) const
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

bool FRPRMaterialEditorBoolPropertiesLayout::GetBoolValue(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	bool value;
	GetIsEnabledPropertyHandle(PropertyHandle)->GetValue(value);
	return (value);
}

void FRPRMaterialEditorBoolPropertiesLayout::SetBoolValue(TSharedRef<IPropertyHandle> PropertyHandle, bool Value)
{
	GetIsEnabledPropertyHandle(PropertyHandle)->SetValue(Value);
}

TSharedPtr<IPropertyHandle> FRPRMaterialEditorBoolPropertiesLayout::GetIsEnabledPropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	return (PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialBool, bIsEnabled)));
}
