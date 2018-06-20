#pragma once
#include "Engine/Texture2DDynamic.h"

class FRPRRenderSystem
{
public:

	void	SetRPRRenderUnit(FRPRRenderUnitPtr RenderUnit);

	void	Rebuild();
	void	Render();

public:

	FORCEINLINE RPR::FContext			GetRPRContext() const			{ return RPRContext; }
	FORCEINLINE RPR::FScene				GetScene() const				{ return RPRScene; }
	FORCEINLINE RPR::FMaterialSystem	GetMaterialSystem() const		{ return RPRMaterialSystem; }
	FORCEINLINE RPRX::FContext			GetRPRXSupportContext() const	{ return RPRXSupportCtx; }
	FORCEINLINE RPRI::FContext			GetRPRIContext() const			{ return RPRIContext; }
	FORCEINLINE RPR::FImageManagerPtr	GetRPRImageManager() const		{ return RPRImageManager; }
	FORCEINLINE FRPRXMaterialLibrary	GetRPRMaterialLibrary() const	{ return RPRXMaterialLibrary; }

private:

	typedef std::map<std::string, rpriExportRprMaterialResult> FMaterialCache;

	FRPRRenderUnitPtr		CurrentRenderUnit;
	FRPRCameraController	ActiveCameraController;

	// RPR Rendered Buffer
	UTexture2DDynamic*		RenderTexture;

	RPR::FContext			RPRContext;
	RPR::FScene				RPRScene;
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