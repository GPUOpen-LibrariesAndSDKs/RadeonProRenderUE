#include "RPRMaterialEditor/PropertiesLayout/SRPRMaterialUVSettings.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/SBoxPanel.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "PropertyHandlerHelpers.h"
#include "Widgets/Layout/SBox.h"

void SRPRMaterialUVSettings::Construct(const FArguments& InArgs)
{
	MaterialMapPropertyHandle = InArgs._MaterialMapPropertyHandle;
	check(MaterialMapPropertyHandle.IsValid());

	ChildSlot
		[
			SNew(SScrollBox)
			+SScrollBox::Slot() // UV Mode
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				[
					GetUVModePropertyHandle()->CreatePropertyNameWidget()
				]
				+SHorizontalBox::Slot()
				[
					GetUVModePropertyHandle()->CreatePropertyValueWidget()
				]
			]
			+SScrollBox::Slot() // Weight
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

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetUVWeightPropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, UVWeight)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetThresholdPropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Threshold)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetUVModePropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, UVMode)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetOriginPropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Origin)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetXAxisPropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, XAxis)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetZAxisPropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, ZAxis)));
}

TSharedPtr<IPropertyHandle> SRPRMaterialUVSettings::GetScalePropertyHandle() const
{
	return (MaterialMapPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Scale)));
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

