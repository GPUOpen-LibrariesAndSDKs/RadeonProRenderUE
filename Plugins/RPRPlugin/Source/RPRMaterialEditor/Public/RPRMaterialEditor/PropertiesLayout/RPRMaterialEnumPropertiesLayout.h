#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"

class FRPRMaterialEnumPropertiesLayout : public IPropertyTypeCustomization
{

public:
	
	static TSharedRef<class IPropertyTypeCustomization>	MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

private:

	TSharedRef<SWidget>		GenerateEnumWidget(TSharedPtr<FString> InItem);
	FText					GetSelectedEnumValue(TSharedRef<IPropertyHandle> PropertyHandle) const;
	void					HandleEnumSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo, TSharedRef<IPropertyHandle> PropertyHandle);

	void					GenerateEnumNames(UEnum* Enum, TArray<TSharedPtr<FString>>& OutEnumNames) const;

	UEnum*					GetEnumTypeFromPropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const;

	TSharedPtr<IPropertyHandle>		GetEnumTypePropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const;
	TSharedPtr<IPropertyHandle>		GetEnumValuePropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const;

private:

	TArray<TSharedPtr<FString>>	EnumOptions;

};