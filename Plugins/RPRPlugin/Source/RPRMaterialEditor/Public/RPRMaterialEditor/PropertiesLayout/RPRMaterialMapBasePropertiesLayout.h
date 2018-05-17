#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "RPRMaterialMapMode.h"
#include "PropertyHandle.h"

class FRPRMaterialMapBasePropertiesLayout : public IPropertyTypeCustomization
{
public:

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}
	
protected:

	virtual TSharedPtr<IPropertyHandle>	GetModePropertyHandle() const = 0;
	virtual TSharedPtr<SWidget> GetConstantPropertyWidget() const = 0;

private:

	TSharedPtr<IPropertyHandle>	GetTexturePropertyHandle() const;

	void FindMaterialModeEnumType();
	void BuildModeAvailables();

	TSharedRef<SWidget>		GenerateModeWidget(TSharedPtr<FString> Item);
	void					HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	FText					GetCurrentModeText() const;

	int32					GetModeIndex() const;
	ERPRMaterialMapMode		GetMode() const;
	void					SetMode(ERPRMaterialMapMode Mode);

	int32					FindModeIndex(TSharedPtr<FString> ModeString) const;

protected:

	TSharedPtr<IPropertyHandle>		CurrentPropertyHandle;
	
private:

	TArray<TSharedPtr<FString>>		ModeAvailables;
	UEnum*							MaterialModeEnumType;

};
