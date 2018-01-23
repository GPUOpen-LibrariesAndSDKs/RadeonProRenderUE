#pragma once

#include "RPRTypedefs.h"
#include "RPREnums.h"

namespace RPR
{
	class FMaterialHelpers
	{
	public:

		static FResult	CreateNode(RPR::FMaterialSystem MaterialSystem, EMaterialNodeType NodeType, FMaterialNode& OutMaterialNode);
		static FResult	DeleteMaterial(RPR::FContext Context, RPR::FMaterial MaterialData);

		template<typename ValueType>
		static FResult	SetNodeParameter(RPR::FMaterial Material, const ValueType& Value);

		/*template<>
		static FResult	SetNodeParameter<UTexture2D*>(RPR::FMaterial, const UTexture2D* Texture);*/
	};

	template<typename ValueType>
	FResult RPR::FMaterialHelpers::SetNodeParameter(RPR::FMaterial Material, const ValueType& Value)
	{
		static_assert(false, "ValueType not supported");
	}

}