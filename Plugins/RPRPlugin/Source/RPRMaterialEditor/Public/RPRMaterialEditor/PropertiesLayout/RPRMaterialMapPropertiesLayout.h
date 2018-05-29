#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "Reply.h"

class FRPRMaterialMapPropertiesLayout : public IPropertyTypeCustomization
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();

	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override {}

protected:

	TSharedPtr<IPropertyHandle> GetTexturePropertyHandle() const;
	TSharedRef<SWidget>			CreateCheckedTexturePropertyWidget() const;
	EVisibility					GetTextureFormatFixerAreaVisibility() const;
	FReply						OnFixTextureFormatButtonClicked();
	UTexture2D*					GetTexture();
	const UTexture2D*			GetTexture() const;

protected:

	TSharedPtr<IPropertyHandle>		CurrentPropertyHandle;

};