#pragma once

#include "UnrealString.h"
#include "RPRTypedefs.h"
#include "RPRHelpers.h"
#include "RPREnums.h"

namespace RPR
{
	class FMaterialHelpers
	{
	public:

		static const TCHAR*	ImageDataInputName;

		static FResult	CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode);
		static FResult	DeleteNode(FMaterialNode& MaterialNode);

		static FResult	CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, 
													UTexture2D* Texture, FMaterialNode& MaterialNode);
	};


}