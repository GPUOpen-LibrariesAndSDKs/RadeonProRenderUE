#pragma once
#include "IPropertyTypeCustomization.h"
#include "SWidget.h"
#include "RPRMaterialMap.h"
#include "RPRMaterialMapBasePropertiesLayout.h"

class FRPRMaterialConstantOrMapPropertiesLayout : public FRPRMaterialConstantOrMapBasePropertiesLayout
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();

protected:

	virtual TSharedPtr<IPropertyHandle>	GetModePropertyHandle() const override;
	virtual TSharedPtr<SWidget> GetConstantPropertyWidget() const override;


private:

	TSharedPtr<IPropertyHandle>	GetConstantPropertyHandle() const;

	FLinearColor			GetConstantColor() const;
	void					SetConstantColor(const FLinearColor& Color);
	FReply					HandleColorBlockClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent);
	void					HandleConstantColorPickChanged(FLinearColor NewColor);

private:

	TAttribute<FLinearColor>		ConstantColorAttribute;

};
