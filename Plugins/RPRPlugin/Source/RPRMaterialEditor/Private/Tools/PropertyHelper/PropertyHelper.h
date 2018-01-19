#pragma once

#include "UnrealType.h"
#include "NameTypes.h"

class FPropertyHelper
{
public:

	static bool	IsPropertyMemberOf(const FPropertyChangedEvent& PropertyChangedEvent, const FString& PropertyName);

};
