/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

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