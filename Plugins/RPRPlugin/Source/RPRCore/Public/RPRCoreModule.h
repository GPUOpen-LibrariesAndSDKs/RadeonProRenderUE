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

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "RPRCoreSystemResources.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRCore, All, All);

// Used to trace rendering steps
DECLARE_LOG_CATEGORY_EXTERN(LogRPRCore_Steps, Verbose, Verbose);

class RPRCORE_API IRPRCore : public IModuleInterface
{

public:
	void StartupModule() override;
	void ShutdownModule() override;

	static IRPRCore& Get()
	{
		return FModuleManager::GetModuleChecked<IRPRCore>("RPRCore");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("RPRCore");
	}

	static FRPRCoreSystemResourcesPtr GetResources()
	{
		return FModuleManager::GetModuleChecked<IRPRCore>("RPRCore").Resources;
	}

private:
	FRPRCoreSystemResourcesPtr Resources;

};

