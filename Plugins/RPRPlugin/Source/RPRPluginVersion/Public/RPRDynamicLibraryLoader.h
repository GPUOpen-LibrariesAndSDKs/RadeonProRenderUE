#pragma once

#include "CoreMinimal.h"

class RPRPLUGINVERSION_API FRPRDynamicLibraryLoader
{
public:

	using FDLLHandle = void*;

	static TArray<FDLLHandle>	LoadLibraries(const FString& DllDirectory, const TArray<FString>& DllNames);
	static void					UnloadLibraries(const TArray<FDLLHandle>& DllHandles);

private:

	static void		OpenDLLNotFoundMessageBox(const TArray<FString>& DllPaths);


};