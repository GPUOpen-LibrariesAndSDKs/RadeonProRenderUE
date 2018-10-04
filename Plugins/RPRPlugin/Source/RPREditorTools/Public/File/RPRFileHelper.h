#pragma once
#include "Containers/UnrealString.h"

class RPREDITORTOOLS_API FRPRFileHelper
{
public:

	// Generate a unique filename according to the other files in the same directory.
	// If the filename is empty, the BaseFilename will be used as a base.
	template<class T>
	static FString	FixFilenameIfInvalid(const FString& FilePath, const FString& BaseFilename)
	{
		static_assert(!TIsPointer<T>::Value, "You must set the template as a class, not a pointer on a class.");
		return FixFilenameIfInvalid(FilePath, T::StaticClass(), BaseFilename);
	}

	static FString	FixFilenameIfInvalid(const FString& FilePath, UClass* ObjectClass, const FString& BaseFilename);

	static bool		DeletePackageIfExists(const FString& AssetPath);
	static FString	GenerateUniqueFilename(const FString& FilePath);

};