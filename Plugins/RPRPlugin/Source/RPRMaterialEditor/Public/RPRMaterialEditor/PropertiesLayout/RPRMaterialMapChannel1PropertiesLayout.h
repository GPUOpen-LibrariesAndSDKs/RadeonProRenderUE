#pragma once
#include "IPropertyTypeCustomization.h"
#include "RPRMaterialMapBasePropertiesLayout.h"

class FRPRMaterialConstantOrMapChannel1PropertiesLayout : public FRPRMaterialConstantOrMapBasePropertiesLayout
{
public:

	static TSharedRef<class IPropertyTypeCustomization>	MakeInstance();

protected:

	virtual TSharedPtr<IPropertyHandle> GetModePropertyHandle() const override;
	virtual TSharedPtr<SWidget> GetConstantPropertyWidget() const override;

private:

	TSharedPtr<IPropertyHandle>	GetConstantPropertyHandle() const;

};