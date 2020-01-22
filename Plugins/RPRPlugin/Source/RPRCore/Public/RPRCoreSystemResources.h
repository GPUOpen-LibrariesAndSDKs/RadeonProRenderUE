/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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

	bool	InitializeRPRRendering();
	bool	InitializeContext();
	bool	InitializeMaterialSystem();
	bool	InitializeContextParameters();
	void	InitializeRPRImageManager();
	void	InitializeRPRXMaterialLibrary();
	bool	LoadTahoeDLL();
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

	bool	bIsInitialized;

	RPR::FPluginId			TahoePluginId;
	int32					NumDevicesCompatible;

	RPR::FContext			RPRContext;
	RPR::FMaterialSystem	RPRMaterialSystem;
	RPR::FImageManagerPtr	RPRImageManager;
	FRPRXMaterialLibrary	RPRXMaterialLibrary;
};

typedef TSharedPtr<FRPRCoreSystemResources> FRPRCoreSystemResourcesPtr;
