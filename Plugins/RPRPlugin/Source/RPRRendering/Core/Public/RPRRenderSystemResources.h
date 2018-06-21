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