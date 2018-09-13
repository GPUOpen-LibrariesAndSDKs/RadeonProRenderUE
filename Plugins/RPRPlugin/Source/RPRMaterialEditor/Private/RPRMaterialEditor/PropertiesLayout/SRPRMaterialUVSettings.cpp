#include "RPRMaterialEditor/PropertiesLayout/SRPRMaterialUVSettings.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "PropertyHandlerHelpers.h"
#include "Widgets/Layout/SBox.h"
#include "Material/RPRMaterialMapUV.h"
#include "Widgets/Layout/SSpacer.h"

void SRPRMaterialUVSettings::Construct(const FArguments& InArgs)
{
	UVSettingsPropertyHandle = InArgs._MaterialMapPropertyHandle;
	check(UVSettingsPropertyHandle.IsValid());

	ChildSlot
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot() // UV Channel
				.AutoWidth()
				[
					GetUChannelPropertyHandle()->CreatePropertyNameWidget()
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5, 0, 0, 0)
				[
					GetUChannelPropertyHandle()->CreatePropertyValueWidget()
				]
				+SHorizontalBox::Slot()
				[
					SNew(SSpacer)
					.Size(FVector2D(20, 0))
				]
				+SHorizontalBox::Slot() // UV Mode
				.AutoWidth()
				[
					GetUVModePropertyHandle()->CreatePropertyNameWidget()
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.Padding(5, 0, 0, 0)
				[
					GetUVModePropertyHandle()->CreatePropertyValueWidget()
				]
			]
			+SScrollBox::Slot() // Weight
			.Padding(0, 2.5f, 0, 2.5f)
			[
				SNew(SBox)
				.Visibility(this, &SRPRMaterialUVSettings::GetPropertyVisibilityFromExpectedUVMode, ETextureUVMode::Triplanar)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						GetUVWeightPropertyHandle()->CreatePropertyNameWidget()
					]
					+SHorizontalBox::Slot()
					[
						GetUVWeightPropertyHandle()->CreatePropertyValueWidget()
					]
				]
			]
			+SScrollBox::Slot() // Origin
			[
				SNew(SBox)
				.Visibility(this, &SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVMode, ETextureUVMode::None)
				[
					FPropertyHandlerHelpers::CreateVectorPropertyWidget(GetOriginPropertyHandle())
				]
			]
			+SScrollBox::Slot() // Threshold
			[
				SNew(SBox)
				.Visibility(this, &SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVModes, TArray<ETextureUVMode>({ETextureUVMode::Triplanar, ETextureUVMode::None}))
				[
					FPropertyHandlerHelpers::CreateVectorPropertyWidget(GetThresholdPropertyHandle())
				]
			]
			+SScrollBox::Slot() // X-Axis
			[
				SNew(SBox)
				.Visibility(this, &SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVMode, ETextureUVMode::None)
				[
					FPropertyHandlerHelpers::CreateVectorPropertyWidget(GetXAxisPropertyHandle())
				]
			]
			+SScrollBox::Slot() // Z-Axis
			[
				SNew(SBox)
				.Visibility(this, &SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVMode, ETextureUVMode::None)
				[
					FPropertyHandlerHelpers::CreateVectorPropertyWidget(GetZAxisPropertyHandle())
				]
			]
			+SScrollBox::Slot() // Scale
			[
				SNew(SBox)
				.Visibility(this, &SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVMode, ETextureUVMode::None)
				[
					FPropertyHandlerHelpers::CreateVector2DPropertyWidget(GetScalePropertyHandle())
				]
			]
		];
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetUChannelPropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, UVChannel)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetUVWeightPropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, UVWeight)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetThresholdPropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, Threshold)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetUVModePropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, UVMode)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetOriginPropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, Origin)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetXAxisPropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, XAxis)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetZAxisPropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, ZAxis)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetScalePropertyHandle() const
{
	return (UVSettingsPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMapUV, Scale)));
}

EVisibility SRPRMaterialUVSettings::GetPropertyVisibilityFromExpectedUVMode(ETextureUVMode ExpectedMode) const
{
	return GetCurrentTextureUVMode() == ExpectedMode ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SRPRMaterialUVSettings::GetPropertyVisibilityFromExpectedUVModes(TArray<ETextureUVMode> ExpectedModes) const
{
	for (int32 i = 0; i < ExpectedModes.Num(); ++i)
	{
		if (GetPropertyVisibilityFromExpectedUVMode(ExpectedModes[i]) == EVisibility::Visible)
		{
			return EVisibility::Visible;
		}
	}
	return EVisibility::Collapsed;
}

EVisibility SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVMode(ETextureUVMode UnexpectedMode) const
{
	return GetCurrentTextureUVMode() != UnexpectedMode ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SRPRMaterialUVSettings::GetPropertyVisibilityFromUnexpectedUVModes(TArray<ETextureUVMode> UnexpectedModes) const
{
	ETextureUVMode uvMode = GetCurrentTextureUVMode();
	for (int32 i = 0; i < UnexpectedModes.Num(); ++i)
	{
		if (UnexpectedModes[i] == uvMode)
		{
			return EVisibility::Collapsed;
		}
	}
	return EVisibility::Visible;
}

ETextureUVMode SRPRMaterialUVSettings::GetCurrentTextureUVMode() const
{
	uint8 uvMode;
	if (GetUVModePropertyHandle()->GetValue(uvMode) == FPropertyAccess::Success)
	{
		return (ETextureUVMode) uvMode;
	}
	return ETextureUVMode::None;
}

