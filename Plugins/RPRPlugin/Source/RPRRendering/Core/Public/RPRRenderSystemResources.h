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
#include "Engine/Texture2DDynamic.h"
#include "RPRTypedefs.h"

class FRPRRenderSystemResources
{
public:

	FRPRRenderSystemResources();

	bool	Initialize();
	void	Shutdown();

public:

	FORCEINLINE RPR::FContext			GetRPRContext() const { return RPRContext; }
	FORCEINLINE RPR::FMaterialSystem	GetMaterialSystem() const { return RPRMaterialSystem; }
	FORCEINLINE RPRX::FContext			GetRPRXSupportContext() const { return RPRXSupportCtx; }
	FORCEINLINE RPRI::FContext			GetRPRIContext() const { return RPRIContext; }
	FORCEINLINE RPR::FImageManagerPtr	GetRPRImageManager() const { return RPRImageManager; }
	FORCEINLINE FRPRXMaterialLibrary	GetRPRMaterialLibrary() const { return RPRXMaterialLibrary; }

private:

	bool	InitializeRenderTexture();
	bool	InitializeRPRRendering();
	bool	InitializeContext();
	bool	InitializeRPRIContext();
	bool	InitializeMaterialSystem();
	bool	InitializeRPRXContext();
	bool	InitializeContextParameters();
	bool	LoadTahoeDLL();

	void	DestroyRPRContext();
	void	DestroyMaterialSystem();
	void	DestroyRPRIContext();
	void	DestroyRPRXSupportContext();
	void	DestroyRenderTexture();

	RPR::FCreationFlags		GetContextCreationFlags() const;
	RPR::FCreationFlags		GetMaxCreationFlags() const;
	RPR_TOOLS_OS			GetCurrentToolOS() const;
	void					LogCompatibleDevices(RPR::FCreationFlags CreationFlags) const;
	int32					CountCompatibleDevices(RPR::FCreationFlags CreationFlags) const;

private:

	typedef std::map<std::string, rpriExportRprMaterialResult> FMaterialCache;

	FPluginId				TahoePluginId;
	int32					NumDevicesCompatible;

	// RPR Rendered Buffer
	UTexture2DDynamic*		RenderTexture;

	RPR::FContext			RPRContext;
	RPR::FMaterialSystem	RPRMaterialSystem;
	RPRX::FContext			RPRXSupportCtx;
	RPRI::FContext			RPRIContext;

	RPR::FImageManagerPtr	RPRImageManager;
	FRPRXMaterialLibrary	RPRXMaterialLibrary;

	// Old system members
	rpr::MaterialLibrary	MaterialLibrary;
	rpr::UMSControl			UMSControl;
	FMaterialCache			MaterialCache;
};
