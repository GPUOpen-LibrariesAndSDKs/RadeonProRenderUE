#pragma once

#include "UnrealString.h"
#include "RPRTypedefs.h"
#include "RPRHelpers.h"
#include "RPREnums.h"
#include "RPRImageManager.h"

namespace RPR
{
	/*
	* Interface between the RPR material native functions and UE4
	*/
	class FMaterialHelpers
	{
	public:

		static const TCHAR*	ImageDataInputName;

		static FResult	CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode);
		static FResult	DeleteNode(FMaterialNode& MaterialNode);

		static FResult	CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, RPR::FImageManager& ImageManager,
													UTexture2D* Texture, FMaterialNode& MaterialNode);
	};


}