#pragma once
#include "RPRTypedefs.h"
#include "RPRXTypedefs.h"

class FRPR_GLTF_Tools
{
public:

	static RPR::FResult	ImportFromGLFT(
		const FString& Filename, 
		RPR::FContext Context, 
		RPR::FMaterialSystem MaterialSystem, 
		RPRX::FContext RPRContext, 
		RPR::FScene& OutScene
	);

	static RPR::FResult ExportToGLTF(
		const FString& Filename, 
		RPR::FContext Context,
		RPR::FMaterialSystem MaterialSystem,
		RPRX::FContext RPRContext,
		const TArray<RPR::FScene>& Scenes
	);

};