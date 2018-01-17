#pragma once

#include "UnrealString.h"
#include "Package.h"
#include "Object.h"

class FResourcesFinder
{
public:

	static UObject*		FindObjectAmongContentAndPluginDirectory(UClass* ObjectClass, const FString& ContentRelativePath, UPackage* OuterPackage = ANY_PACKAGE);

	template<typename T>
	static T*			FindObjectAmongContentAndPluginDirectory(const FString& ContentRelativePath, UPackage* OuterPackage = ANY_PACKAGE)
	{
		static_assert(std::is_base_of<UObject, T>::value, "T must inherit from UObject");
		return (Cast<T>(FindObjectAmongContentAndPluginDirectory(T::StaticClass(), ContentRelativePath, OuterPackage)));
	}

private:

	static UObject*		FindObjectInPluginDirectory(UClass* ObjectClass, UPackage* OuterPackage, const FString& ContentRelativePath);
	static UObject*		FindObjectInGameDirectory(UClass* ObjectClass, UPackage* OuterPackage, const FString& ContentRelativePath);
	static UObject*		FindObjectInDirectory(UClass* ObjectClass, UPackage* OuterPackage, const FString& Path);

};