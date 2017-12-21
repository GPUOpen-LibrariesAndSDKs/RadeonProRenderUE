#pragma once

#include "SharedPointer.h"
#include "SWidget.h"

class IUVProjection
{
public:
	virtual TSharedRef<SWidget>	TakeWidget() = 0;
};

typedef TSharedPtr<IUVProjection>	IUVProjectionPtr;