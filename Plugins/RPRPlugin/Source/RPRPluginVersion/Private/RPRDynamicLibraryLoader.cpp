#include "RPRDynamicLibraryLoader.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "Internationalization/Text.h"

#define LOCTEXT_NAMESPACE "RPRDynamicLibraryLoader"

TArray<FRPRDynamicLibraryLoader::FDLLHandle> FRPRDynamicLibraryLoader::LoadLibraries(const FString& DllDirectory, const TArray<FString>& DllNames)
{
	TArray<FString> unloadableDLLs;
	TArray<FDLLHandle> dllHandles;

	FPlatformProcess::PushDllDirectory(*DllDirectory);
	{
		for (int32 i = 0; i < DllNames.Num(); ++i)
		{
			FDLLHandle handle = FPlatformProcess::GetDllHandle(*DllNames[i]);

			if (handle != nullptr)
			{
				dllHandles.Add(handle);
			}
			else
			{
				unloadableDLLs.Add(FPaths::Combine(DllDirectory, DllNames[i]));
			}
		}
	}
	FPlatformProcess::PopDllDirectory(*DllDirectory);

	if (unloadableDLLs.Num() > 0)
	{
		OpenDLLNotFoundMessageBox(unloadableDLLs);
		UnloadLibraries(dllHandles);

		dllHandles.Empty();
	}

	return (dllHandles);
}

void FRPRDynamicLibraryLoader::UnloadLibraries(const TArray<FDLLHandle>& DllHandles)
{
	for (int32 i = 0; i < DllHandles.Num(); ++i)
	{
		FPlatformProcess::FreeDllHandle(DllHandles[i]);
	}
}

void FRPRDynamicLibraryLoader::OpenDLLNotFoundMessageBox(const TArray<FString>& DllPaths)
{
	FString paths;
	for (int32 i = 0; i < DllPaths.Num(); ++i)
	{
		paths += TEXT("- ") + DllPaths[i];
		if (i + 1 < DllPaths.Num())
		{
			paths += TEXT("\n");
		}
	}

	FFormatNamedArguments args;
	args.Add(TEXT("Paths"), FText::FromString(paths));
	FText message = FText::Format(LOCTEXT("CannotLoadFilesMessage", "Cannot load files :\n{Paths}\n\nRPR won't be able to work correctly."), args);

	FText titleBox = FText::FromString("Cannot load RPR files");
	FMessageDialog::Open(EAppMsgType::Ok, message, &titleBox);
}

#undef LOCTEXT_NAMESPACE