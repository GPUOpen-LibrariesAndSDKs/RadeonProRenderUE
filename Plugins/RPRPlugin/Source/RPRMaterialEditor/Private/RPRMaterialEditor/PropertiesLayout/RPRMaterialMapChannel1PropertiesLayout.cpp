#include "RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialCoMChannel1.h"
#include "SBox.h"
#include "SNumericEntryBox.h"
#include "Private/PropertyHandleImpl.h"

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

TOptional<float> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantValue() const
{
	float value;
	GetConstantPropertyHandle()->GetValue(value);
	return (TOptional<float>(value));
}

TSharedPtr<SWidget> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantPropertyWidget() const
{
	TArray<UObject*> objects;
	CurrentPropertyHandle->GetOuterObjects(objects);
	if (objects.Num() == 0)
	{
		return (nullptr);
	}

	FRPRMaterialCoMChannel1* materialCoM = (FRPRMaterialCoMChannel1*)CurrentPropertyHandle->GetValueBaseAddress((uint8*)objects[0]);

	FNumericRestriction<float>& constantRestriction = materialCoM->GetConstantRestriction();

	float value;
	GetConstantPropertyHandle()->GetValue(value);

	TOptional<float> minValue;
	TOptional<float> maxValue;

	if (constantRestriction.IsConstrainedByMinimum())
	{
		minValue = TOptional<float>(constantRestriction.GetMinimum());
	}
	if (constantRestriction.IsConstrainedByMaximum())
	{
		maxValue = TOptional<float>(constantRestriction.GetMaximum());
	}

	return
		SNew(SNumericEntryBox<float>)
			.MinDesiredValueWidth(90.0f)
			.Value(this, &FRPRMaterialCoMChannel1PropertiesLayout::GetConstantValue)
			.AllowSpin(true)
			.OnValueChanged(this, &FRPRMaterialCoMChannel1PropertiesLayout::OnConstantValueChanged)
			.MinValue(minValue)
			.MaxValue(maxValue)
			.MinSliderValue(minValue)
			.MaxSliderValue(maxValue);
}

void FRPRMaterialCoMChannel1PropertiesLayout::OnConstantValueChanged(float NewValue)
{
	GetConstantPropertyHandle()->SetValue(NewValue);
}
