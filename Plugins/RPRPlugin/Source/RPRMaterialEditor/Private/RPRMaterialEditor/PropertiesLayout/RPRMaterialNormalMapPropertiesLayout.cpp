#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialNormalMapPropertiesLayout.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SBox.h"

#define LOCTEXT_NAMESPACE "RPRMaterialNormalMapPropertiesLayout"

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
		.Padding(0.0f, 0.0f, 10.0f, 0.0f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			[
				CreateCheckedTexturePropertyWidget()
			]
			+SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.HAlign(HAlign_Left)
			.AutoHeight()
			.Padding(0.0f, 5.0f)
			[
				GetNormalMapModePropertyWidget().ToSharedRef()
			]
			+SVerticalBox::Slot()
			.VAlign(VAlign_Top)
			.AutoHeight()
			[
				SNew(SBox)
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
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(0.0f, 0.0f, 10.0f, 0.0f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("BumpScaleLabel", "Bump Scale"))
		]
		+SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNew(SNumericEntryBox<float>)
			.ToolTipText(LOCTEXT("BumpScaleTooltip", "The bump scale factor"))
			.MinDesiredValueWidth(90.0f)
			.AllowSpin(true)
			.Value(this, &FRPRMaterialNormalMapPropertiesLayout::GetBumpScaleValue)
			.OnValueChanged(this, &FRPRMaterialNormalMapPropertiesLayout::OnBumpScaleValueChanged)
			.MinValue(minValue)
			.MinSliderValue(minValue)
			.MaxSliderValue(maxValue)
		];
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

#undef LOCTEXT_NAMESPACE