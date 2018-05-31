#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "RPRMaterialParameterBasePropertyLayout.h"

class FRPRMaterialEnumPropertiesLayout : public FRPRMaterialParameterBasePropertyLayout
{

public:
	
	static TSharedRef<class IPropertyTypeCustomization>	MakeInstance();

protected:

	virtual TSharedRef<SWidget> GetPropertyValueRowWidget() override;


private:

	TSharedRef<SWidget>		GenerateEnumWidget(TSharedPtr<FString> InItem);
	FText					GetSelectedEnumValue() const;
	void					HandleEnumSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);

	void					GenerateEnumNames(UEnum* Enum, TArray<TSharedPtr<FString>>& OutEnumNames) const;

	UEnum*					GetEnumTypeFromPropertyHandle() const;

	TSharedPtr<IPropertyHandle>		GetEnumTypePropertyHandle() const;
	TSharedPtr<IPropertyHandle>		GetEnumValuePropertyHandle() const;

private:

	TArray<TSharedPtr<FString>>	EnumOptions;

};