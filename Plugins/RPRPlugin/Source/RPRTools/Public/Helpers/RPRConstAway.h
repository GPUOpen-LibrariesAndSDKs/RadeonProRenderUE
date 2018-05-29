#pragma once
#include "Platform.h"

namespace RPR
{
	/* 
	* Only used to not duplicate non-const version of the exact same const function.
	* ie : 
	* - const FData& Class::GetData() const { ... }
	* - FData& Class::GetData() 
	* { 
	*   const Class* thisConst = this;
	*   return (ConstRefAway(thisConst->GetData());
	* }
	*
	* YOU SHOULD NOT USE THAT FOR ANY OTHER REASON!
	*/
	template<typename T>
	T& ConstRefAway(const T& item)
	{
		return (const_cast<T&>(item));
	}

	template<typename T>
	T* ConstRefAway(const T* item)
	{
		return (const_cast<T*>(item));
	}
}