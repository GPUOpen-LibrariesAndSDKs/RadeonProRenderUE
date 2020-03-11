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

#include "Typedefs/RPRTypedefs.h"
#include "Material/RPRXMaterialLibrary.h"
#include "ImageManager/RPRImageManager.h"
#include "RprTools.h"

class RPRCORE_API FRPRCoreSystemResources
{
public:

	FRPRCoreSystemResources();

	bool	Initialize();
	void	Shutdown();
	bool	IsInitialized() const;
public:

	FORCEINLINE RPR::FContext			GetRPRContext() const { return RPRContext; }
	FORCEINLINE RPR::FMaterialSystem	GetMaterialSystem() const { return RPRMaterialSystem; }
	FORCEINLINE RPR::FImageManagerPtr	GetRPRImageManager() const { return RPRImageManager; }
	FORCEINLINE FRPRXMaterialLibrary&	GetRPRMaterialLibrary() { return RPRXMaterialLibrary; }
	FORCEINLINE int32					GetNumDevicesCompatible() const { return NumDevicesCompatible; }

private:

	bool	LoadLibraries();
	bool	InitializeContextEnvirontment();
	bool	InitializeContext();
	bool	InitializeMaterialSystem();
	bool	InitializeContextParameters();
	void	InitializeRPRImageManager();
	void	InitializeRPRXMaterialLibrary();
	bool	LoadRprDLL(const FString Library, RPR::FPluginId &libId);
	bool	LoadImageFilterDLL();

	void	DestroyRPRContext();
	void	DestroyMaterialSystem();
	void	DestroyRPRImageManager();
	void	DestroyRPRXMaterialLibrary();

	RPR::FCreationFlags		GetContextCreationFlags() const;
	RPR::FCreationFlags		GetMaxCreationFlags() const;
	RPR_TOOLS_OS			GetCurrentToolOS() const;
	void					LogCompatibleDevices(RPR::FCreationFlags CreationFlags) const;
	int32					CountCompatibleDevices(RPR::FCreationFlags CreationFlags) const;

private:

	bool					bIsInitialized;
	ERenderType				CurrentContextType;

	RPR::FPluginId			CurrentPluginId;
	RPR::FPluginId			TahoePluginId;
	RPR::FPluginId			HybridPluginId;
	int32					NumDevicesCompatible;

	RPR::FContext			RPRContext;
	RPR::FMaterialSystem	RPRMaterialSystem;
	RPR::FImageManagerPtr	RPRImageManager;
	FRPRXMaterialLibrary	RPRXMaterialLibrary;
};

typedef TSharedPtr<FRPRCoreSystemResources> FRPRCoreSystemResourcesPtr;
