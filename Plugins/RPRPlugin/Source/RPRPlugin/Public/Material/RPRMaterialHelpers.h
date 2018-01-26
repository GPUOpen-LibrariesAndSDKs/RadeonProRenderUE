#pragma once

#include "RPRTypedefs.h"
#include "RPRHelpers.h"
#include "RPREnums.h"

namespace RPR
{
	class FMaterialHelpers
	{
	public:

		static const FName	ImageDataInputName;

		static FResult	CreateNode(FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode);
		static FResult	DeleteNode(FMaterialNode& MaterialNode);

		static FResult	CreateImageNode(RPR::FContext RPRContext, FMaterialSystem MaterialSystem, UTexture2D* Texture, FMaterialNode& OutMaterialNode);
	};


}