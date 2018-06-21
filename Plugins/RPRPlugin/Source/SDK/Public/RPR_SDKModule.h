// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FRPR_SDKModule : public IModuleInterface
{
public:
	virtual void	StartupModule() override;
	virtual void	ShutdownModule() override;
	
	static RPR_SDK_API FString	GetDLLsDirectory();

private:

	void	PreloadDLLs(const TArray<FString>& DllNames);
	void	UnloadDLLs();

private:

	using FDLLHandle = void*;
	TArray<FDLLHandle>		dllHandles;

};