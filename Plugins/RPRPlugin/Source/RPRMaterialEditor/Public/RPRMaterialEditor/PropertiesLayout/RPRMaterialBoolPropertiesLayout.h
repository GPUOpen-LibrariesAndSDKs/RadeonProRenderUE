#pragma once
#include "IPropertyTypeCustomization.h"
#include "SlateTypes.h"
#include "RPRMaterialBool.h"
#include "RPRMaterialParameterBasePropertyLayout.h"

class FRPRMaterialBoolPropertiesLayout : public FRPRMaterialParameterBasePropertyLayout
{

public:

	static TSharedRef<IPropertyTypeCustomization>	MakeInstance();
	
protected:

	virtual TSharedRef<SWidget> GetPropertyValueRowWidget() override;

private:

	void			OnCheckStateChanged(ECheckBoxState CheckboxState);
	ECheckBoxState	IsChecked() const;

	bool			GetBoolValue() const;
	void			SetBoolValue(bool Value);

	TSharedPtr<IPropertyHandle>		GetIsEnabledPropertyHandle() const;


};
