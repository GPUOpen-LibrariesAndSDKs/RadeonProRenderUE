#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialNormalMapPropertiesLayout.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SNumericEntryBox.h"

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialNormalMapPropertiesLayout::MakeInstance()
{
	return MakeShareable(new FRPRMaterialNormalMapPropertiesLayout);
}

TSharedRef<SWidget> FRPRMaterialNormalMapPropertiesLayout::GetPropertyValueRowWidget()
{
	return
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			[
				CreateCheckedTexturePropertyWidget()
			]
			+SVerticalBox::Slot()
			[
				GetNormalMapModePropertyWidget().ToSharedRef()
			]
			+SVerticalBox::Slot()
			[
				SNew(SBorder)
				.Visibility(this, &FRPRMaterialNormalMapPropertiesLayout::GetBumpScalePropertyWidgetVisibility)
				[
					GetBumpScalePropertyWidget().ToSharedRef()
				]
			]
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.FillWidth(1.0f)
		[
			CreateUVSettingsPropertyWidget()
		]
	;
}

TSharedPtr<IPropertyHandle> FRPRMaterialNormalMapPropertiesLayout::GetNormalMapModePropertyHandle() const
{
	return CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialNormalMap, Mode));
}

TSharedPtr<IPropertyHandle> FRPRMaterialNormalMapPropertiesLayout::GetBumpScalePropertyHandle() const
{
	return CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialNormalMap, BumpScale));
}

TSharedPtr<SWidget> FRPRMaterialNormalMapPropertiesLayout::GetNormalMapModePropertyWidget() const
{
	return GetNormalMapModePropertyHandle()->CreatePropertyValueWidget();
}

TSharedPtr<SWidget> FRPRMaterialNormalMapPropertiesLayout::GetBumpScalePropertyWidget() const
{
	constexpr float minValue = 0.0f;
	constexpr float maxValue = 1.0f;

	return 
		SNew(SNumericEntryBox<float>)
		.MinDesiredValueWidth(90.0f)
		.AllowSpin(true)
		.Value(this, &FRPRMaterialNormalMapPropertiesLayout::GetBumpScaleValue)
		.OnValueChanged(this, &FRPRMaterialNormalMapPropertiesLayout::OnBumpScaleValueChanged)
		.MinValue(minValue)
		.MaxValue(maxValue)
		.MinSliderValue(minValue)
		.MaxSliderValue(maxValue);
}

EVisibility FRPRMaterialNormalMapPropertiesLayout::GetBumpScalePropertyWidgetVisibility() const
{
	return GetNormalMapMode() == ENormalMapMode::Bump ? EVisibility::Visible : EVisibility::Collapsed;
}

TOptional<float> FRPRMaterialNormalMapPropertiesLayout::GetBumpScaleValue() const
{
	float bumpScale;
	GetBumpScalePropertyHandle()->GetValue(bumpScale);
	return TOptional<float>(bumpScale);
}

void FRPRMaterialNormalMapPropertiesLayout::OnBumpScaleValueChanged(float NewValue)
{
	GetBumpScalePropertyHandle()->SetValue(NewValue);
}

ENormalMapMode FRPRMaterialNormalMapPropertiesLayout::GetNormalMapMode() const
{
	uint8 normalMapMode;
	GetNormalMapModePropertyHandle()->GetValue(normalMapMode);
	return (ENormalMapMode) normalMapMode;
}
