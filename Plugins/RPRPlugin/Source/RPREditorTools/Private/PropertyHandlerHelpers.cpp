#include "PropertyHandlerHelpers.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SScrollBox.h"

TSharedRef<SWidget> FPropertyHandlerHelpers::CreateVector2DPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandle, const FText& NameToOverride)
{
	TSharedPtr<SSpinBox<float>> comp1SpinBox;
	TSharedPtr<SSpinBox<float>> comp2SpinBox;

	TSharedRef<SWidget> widget =
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(0, 0, 10, 5.f)
		.AutoWidth()
		[
			PropertyHandle->CreatePropertyNameWidget(NameToOverride)
		]
		+SHorizontalBox::Slot()
		.Padding(0, 0, 10, 5.f)
		[
			SAssignNew(comp1SpinBox, SSpinBox<float>)
			.Value(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateStatic(&FPropertyHandlerHelpers::GetVector2DComponentValue, PropertyHandle, EAxis::X)))
			.OnValueChanged(SSpinBox<float>::FOnValueChanged::CreateStatic(&FPropertyHandlerHelpers::OnVector2DComponentValueChanged, PropertyHandle, EAxis::X))
		]
		+SHorizontalBox::Slot()
		.Padding(0, 0, 0, 5.f)
		[
			SAssignNew(comp2SpinBox, SSpinBox<float>)
			.Value(TAttribute<float>::Create(TAttribute<float>::FGetter::CreateStatic(&FPropertyHandlerHelpers::GetVector2DComponentValue, PropertyHandle, EAxis::Y)))
			.OnValueChanged(SSpinBox<float>::FOnValueChanged::CreateStatic(&FPropertyHandlerHelpers::OnVector2DComponentValueChanged, PropertyHandle, EAxis::Y))
		]
	;

	if (PropertyHandle->HasMetaData(TEXT("UIMin")))
	{
		float minValue = PropertyHandle->GetFLOATMetaData(TEXT("UIMin"));
		comp1SpinBox->SetMinValue(minValue);
		comp2SpinBox->SetMinValue(minValue);
	}
	else
	{
		comp1SpinBox->SetMinValue(TAttribute<TOptional<float>>());
		comp2SpinBox->SetMinValue(TAttribute<TOptional<float>>());
	}

	if (PropertyHandle->HasMetaData(TEXT("UIMax")))
	{
		float maxValue = PropertyHandle->GetFLOATMetaData(TEXT("UIMax"));
		comp1SpinBox->SetMaxValue(maxValue);
		comp2SpinBox->SetMaxValue(maxValue);
	}
	else
	{
		comp1SpinBox->SetMaxValue(TAttribute<TOptional<float>>());
		comp2SpinBox->SetMaxValue(TAttribute<TOptional<float>>());
	}

	return widget;
}

TSharedRef<SWidget> FPropertyHandlerHelpers::CreateVectorPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandle, const FText& NameToOverride)
{
	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(0, 0, 10, 5.f)
		.AutoWidth()
		[
			PropertyHandle->CreatePropertyNameWidget(NameToOverride)
		]
		+SHorizontalBox::Slot()
		.Padding(0, 0, 0, 5.f)
		[
			SNew(SBox)
			.MinDesiredWidth(200.f)
			[
				SNew(SVectorInputBox)
				.bColorAxisLabels(true)
				.AllowResponsiveLayout(false)
				.AllowSpin(true)
				.X(TAttribute<TOptional<float>>::Create(TAttribute<TOptional<float>>::FGetter::CreateStatic(&FPropertyHandlerHelpers::GetVectorComponent, PropertyHandle, 0)))
				.Y(TAttribute<TOptional<float>>::Create(TAttribute<TOptional<float>>::FGetter::CreateStatic(&FPropertyHandlerHelpers::GetVectorComponent, PropertyHandle, 1)))
				.Z(TAttribute<TOptional<float>>::Create(TAttribute<TOptional<float>>::FGetter::CreateStatic(&FPropertyHandlerHelpers::GetVectorComponent, PropertyHandle, 2)))
				.OnXCommitted(FOnFloatValueCommitted::CreateStatic(&FPropertyHandlerHelpers::SetVectorComponent, PropertyHandle, EAxis::X))
				.OnYCommitted(FOnFloatValueCommitted::CreateStatic(&FPropertyHandlerHelpers::SetVectorComponent, PropertyHandle, EAxis::Y))
				.OnZCommitted(FOnFloatValueCommitted::CreateStatic(&FPropertyHandlerHelpers::SetVectorComponent, PropertyHandle, EAxis::Z))
			]
		]
	;
}

TOptional<float> FPropertyHandlerHelpers::GetVectorComponent(TSharedPtr<IPropertyHandle> PropertyHandle, int32 ComponentIndex)
{
	FVector value;
	if (PropertyHandle->GetValue(value) != FPropertyAccess::Fail)
	{
		return (value.Component(ComponentIndex));
	}
	return (TOptional<float>());
}

void FPropertyHandlerHelpers::SetVectorComponent(float Value, ETextCommit::Type CommitType, TSharedPtr<IPropertyHandle> PropertyHandle, EAxis::Type AxisType)
{
	FVector value;
	if (PropertyHandle->GetValue(value) != FPropertyAccess::Fail)
	{
		value.SetComponentForAxis(AxisType, Value);
		PropertyHandle->SetValue(value);
	}
}

float FPropertyHandlerHelpers::GetVector2DComponentValue(TSharedPtr<IPropertyHandle> Vector2PropertyHandle, EAxis::Type AxisType)
{
	FVector2D value;
	Vector2PropertyHandle->GetValue(value);
	if (AxisType == EAxis::X)
	{
		return value.X;
	}
	else if (AxisType == EAxis::Y)
	{
		return value.Y;
	}
	return (0.0f);
}

void FPropertyHandlerHelpers::OnVector2DComponentValueChanged(float NewValue, TSharedPtr<IPropertyHandle> Vector2PropertyHandle, EAxis::Type AxisType)
{
	FVector2D value;
	if (Vector2PropertyHandle->GetValue(value) != FPropertyAccess::Fail)
	{
		if (AxisType == EAxis::X)
		{
			value.X = NewValue;
		}
		else if (AxisType == EAxis::Y)
		{
			value.Y = NewValue;
		}

		Vector2PropertyHandle->SetValue(value);
	}
}