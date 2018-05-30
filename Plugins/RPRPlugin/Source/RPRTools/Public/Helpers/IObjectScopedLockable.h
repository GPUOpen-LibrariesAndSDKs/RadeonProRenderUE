#pragma once

#include "CriticalSection.h"

class IObjectScopedLockable
{
public:
	virtual ~IObjectScopedLockable() {}

	virtual FCriticalSection&	GetCriticalSection() = 0;
};