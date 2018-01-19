#include "PropertyHelper.h"

bool FPropertyHelper::IsPropertyMemberOf(const FPropertyChangedEvent& PropertyChangedEvent, const FString& PropertyName)
{
	return (PropertyChangedEvent.MemberProperty->GetName() == PropertyName);
}
