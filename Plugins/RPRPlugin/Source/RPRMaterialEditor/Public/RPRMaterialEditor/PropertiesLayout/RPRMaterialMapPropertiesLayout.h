#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "Reply.h"
#include "RPRMaterialParameterBasePropertyLayout.h"

class FRPRMaterialMapPropertiesLayout : public FRPRMaterialParameterBasePropertyLayout
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();

protected:

	virtual TSharedRef<SWidget> GetPropertyValueRowWidget() override;

	TSharedPtr<IPropertyHandle> GetTexturePropertyHandle() const;
	TSharedRef<SWidget>			CreateCheckedTexturePropertyWidget() const;
	EVisibility					GetTextureFormatFixerAreaVisibility() const;
	FReply						OnFixTextureFormatButtonClicked();
	UTexture2D*					GetTexture();
	const UTexture2D*			GetTexture() const;

};