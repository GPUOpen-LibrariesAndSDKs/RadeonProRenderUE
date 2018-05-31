#include "RPRMaterialBoolPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "SCheckBox.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialBoolPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialBoolPropertiesLayout()));
}

TSharedRef<SWidget> FRPRMaterialBoolPropertiesLayout::GetPropertyValueRowWidget()
{
	return
		SNew(SCheckBox)
		.OnCheckStateChanged(this, &FRPRMaterialBoolPropertiesLayout::OnCheckStateChanged)
		.IsChecked(this, &FRPRMaterialBoolPropertiesLayout::IsChecked);
}

void FRPRMaterialBoolPropertiesLayout::OnCheckStateChanged(ECheckBoxState CheckboxState)
{
	TArray<UObject*> objects;
	CurrentPropertyHandle->GetOuterObjects(objects);

	for (int32 i = 0; i < objects.Num(); ++i)
	{
		SetBoolValue(CheckboxState == ECheckBoxState::Checked);
	}
}

ECheckBoxState FRPRMaterialBoolPropertiesLayout::IsChecked() const
{
	TArray<UObject*> objects;
	CurrentPropertyHandle->GetOuterObjects(objects);

	bool bIsInitialized = false;
	bool bIsChecked = false;

	for (int32 i = 0; i < objects.Num(); ++i)
	{
		bool parameterBoolValue = GetBoolValue();
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

bool FRPRMaterialBoolPropertiesLayout::GetBoolValue() const
{
	bool value;
	GetIsEnabledPropertyHandle()->GetValue(value);
	return (value);
}

void FRPRMaterialBoolPropertiesLayout::SetBoolValue(bool Value)
{
	GetIsEnabledPropertyHandle()->SetValue(Value);
}

TSharedPtr<IPropertyHandle> FRPRMaterialBoolPropertiesLayout::GetIsEnabledPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialBool, bIsEnabled)));
}
