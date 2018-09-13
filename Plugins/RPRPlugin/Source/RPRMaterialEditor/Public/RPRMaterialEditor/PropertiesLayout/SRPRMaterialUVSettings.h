#pragma once

#include "SRPRMaterialUVSettings.h"
#include "Widgets/SCompoundWidget.h"
#include "PropertyHandle.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Containers/Map.h"

class SRPRMaterialUVSettings : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SRPRMaterialUVSettings) {}

		SLATE_ARGUMENT(TSharedPtr<IPropertyHandle>, MaterialMapPropertyHandle)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:

	TSharedPtr<IPropertyHandle>	GetUChannelPropertyHandle() const;
	TSharedPtr<IPropertyHandle>	GetUVWeightPropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetThresholdPropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetUVModePropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetOriginPropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetXAxisPropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetZAxisPropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetRotationPropertyHandle() const;
	TSharedPtr<IPropertyHandle> GetScalePropertyHandle() const;

	EVisibility	GetPropertyVisibilityFromExpectedUVMode(ETextureUVMode ExpectedMode) const;
	EVisibility	GetPropertyVisibilityFromExpectedUVModes(TArray<ETextureUVMode> ExpectedModes) const;
	EVisibility	GetPropertyVisibilityFromUnexpectedUVMode(ETextureUVMode UnepectedMode) const;
	EVisibility	GetPropertyVisibilityFromUnexpectedUVModes(TArray<ETextureUVMode> UnexpectedModes) const;
	ETextureUVMode	GetCurrentTextureUVMode() const;

private:

	TSharedPtr<IPropertyHandle> UVSettingsPropertyHandle;
};
