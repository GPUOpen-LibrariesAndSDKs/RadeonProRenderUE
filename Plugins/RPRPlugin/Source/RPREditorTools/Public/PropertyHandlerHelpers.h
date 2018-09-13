#pragma once
#include "PropertyHandle.h"
#include "Templates/SharedPointer.h"
#include "Widgets/SWidget.h"
#include "Math/Axis.h"

class RPREDITORTOOLS_API FPropertyHandlerHelpers : public TSharedFromThis<FPropertyHandlerHelpers>
{
public:
	
	static TSharedRef<SWidget>		CreateVector2DPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandle, const FText& NameToOverride = FText::GetEmpty());
	static TSharedRef<SWidget>		CreateVectorPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandle, const FText& NameToOverride = FText::GetEmpty());

private:

	static TOptional<float>		GetVectorComponent(TSharedPtr<IPropertyHandle> PropertyHandle, int32 ComponentIndex);
	static void					SetVectorComponent(float Value, ETextCommit::Type CommitType, TSharedPtr<IPropertyHandle> PropertyHandle, EAxis::Type AxisType);
	static float				GetVector2DComponentValue(TSharedPtr<IPropertyHandle> Vector2PropertyHandle, EAxis::Type AxisType);
	static void					OnVector2DComponentValueChanged(float NewValue, TSharedPtr<IPropertyHandle> Vector2PropertyHandle, EAxis::Type AxisType);

};