#pragma once

namespace RPR
{
	/* 
	* Only used to not duplicate non-const version of the exact same const function.
	* ie : 
	* - const FData& GetDataConst() const { ... }
	* - FData& GetData() { return (ConstRefAway(GetDataConst()); }
	*/
	template<typename T>
	T& ConstRefAway(const T& item)
	{
		return (const_cast<T&>(item));
	}
}