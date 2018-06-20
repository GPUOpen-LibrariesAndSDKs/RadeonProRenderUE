#pragma once
#include "SharedPointer.h"

// Represents a system that can be plugged into the RenderSystem
// to manage what will be rendered in the RPR viewport
// See it as a system that will describe what should be displayed in the RPR viewport
class FRPRRenderUnit
{
public:

	virtual FName	GetName() = 0;

};

typedef TSharedPtr<FRPRRenderUnit> FRPRRenderUnitPtr;