#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "RPRMaterialMap.h"

class FRPRMaterialMapPropertiesLayout : public IPropertyTypeCustomization
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

private:

	void FindMaterialModeEnumType();
	void BuildModeAvailables();

	TSharedRef<SWidget>		GenerateModeWidget(TSharedPtr<FString> Item);
	void					HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	FText					GetCurrentModeText() const;

	int32					GetModeIndex() const;
	ERPRMaterialMapMode		GetMode() const;
	void					SetMode(ERPRMaterialMapMode Mode);

	FLinearColor			GetConstantColor() const;
	void					SetConstantColor(const FLinearColor& Color);
	FReply					HandleColorBlockClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	void					HandleConstantColorPickChanged(FLinearColor NewColor);

	TSharedPtr<IPropertyHandle>	GetModePropertyHandle() const;
	TSharedPtr<IPropertyHandle>	GetConstantPropertyHandle() const;
	TSharedPtr<IPropertyHandle>	GetTexturePropertyHandle() const;
	
	int32					FindModeIndex(TSharedPtr<FString> ModeString) const;

private:

	TArray<TSharedPtr<FString>>		ModeAvailables;
	UEnum*							MaterialModeEnumType;
	TAttribute<FLinearColor>		ConstantColorAttribute;
	TSharedPtr<IPropertyHandle>		CurrentPropertyHandle;

};
