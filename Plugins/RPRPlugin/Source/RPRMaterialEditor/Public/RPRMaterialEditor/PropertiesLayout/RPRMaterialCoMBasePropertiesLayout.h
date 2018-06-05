#pragma once
#include "SWidget.h"
#include "RPRMaterialMapMode.h"
#include "PropertyHandle.h"
#include "RPRMaterialMapPropertiesLayout.h"

class FRPRMaterialCoMBasePropertiesLayout : public FRPRMaterialMapPropertiesLayout
{
protected:

	virtual TSharedRef<SWidget> GetPropertyValueRowWidget() override;

	virtual TSharedPtr<IPropertyHandle>	GetModePropertyHandle() const = 0;
	virtual TSharedPtr<SWidget> GetConstantPropertyWidget() const = 0;



private:

	void FindMaterialModeEnumType();
	void BuildModeAvailables();

	TSharedRef<SWidget>		GenerateModeWidget(TSharedPtr<FString> Item);
	void					HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	FText					GetCurrentModeText() const;

	int32					GetModeIndex() const;
	ERPRMaterialMapMode		GetMode() const;
	void					SetMode(ERPRMaterialMapMode Mode);

	int32					FindModeIndex(TSharedPtr<FString> ModeString) const;

private:

	TArray<TSharedPtr<FString>>		ModeAvailables;
	UEnum*							MaterialModeEnumType;

};
