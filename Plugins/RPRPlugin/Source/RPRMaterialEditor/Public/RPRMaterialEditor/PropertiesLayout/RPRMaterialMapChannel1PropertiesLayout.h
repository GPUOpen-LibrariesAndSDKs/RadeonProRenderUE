#pragma once
#include "IPropertyTypeCustomization.h"
#include "RPRMaterialCoMBasePropertiesLayout.h"

class FRPRMaterialCoMChannel1PropertiesLayout : public FRPRMaterialCoMBasePropertiesLayout
{
public:

	static TSharedRef<class IPropertyTypeCustomization>	MakeInstance();

protected:

	virtual TSharedPtr<IPropertyHandle> GetModePropertyHandle() const override;
	virtual TSharedPtr<SWidget> GetConstantPropertyWidget() const override;

private:

	TSharedPtr<IPropertyHandle>	GetConstantPropertyHandle() const;

};