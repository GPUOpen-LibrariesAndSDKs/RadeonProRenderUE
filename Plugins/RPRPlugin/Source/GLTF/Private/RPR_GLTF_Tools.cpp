#include "RPR_GLTF_Tools.h"
#include "ProRenderGLTF.h"
#include "Array.h"

RPR::FResult FRPR_GLTF_Tools::ImportFromGLFT(
	const FString& Filename, 
	RPR::FContext Context,
	RPR::FMaterialSystem MaterialSystem, 
	RPRX::FContext RPRContext, 
	RPR::FScene& OutScene)
{
	return rprImportFromGLTF(TCHAR_TO_ANSI(*Filename), Context, MaterialSystem, RPRContext, &OutScene);
}

RPR::FResult FRPR_GLTF_Tools::ExportToGLTF(
	const FString& Filename, 
	RPR::FContext Context, 
	RPR::FMaterialSystem MaterialSystem, 
	RPRX::FContext RPRContext, 
	const TArray<RPR::FScene>& Scenes)
{
	return rprExportToGLTF(TCHAR_TO_ANSI(*Filename), Context, MaterialSystem, RPRContext, Scenes.GetData(), Scenes.Num());
}

