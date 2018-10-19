#pragma once
#include "Factories/Setters/RPRMaterialMapSetter.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{

			class FRPRMaterialNormalMapSetter : public FRPRMaterialMapSetter
			{
			protected:

				virtual bool SetParameterValue(FSerializationContext& SerializationCtx, FRPRUberMaterialParameterBase* UberParameter, RPRX::EMaterialParameterType ParameterType, RPRX::FParameter Parameter) override;

			private:

				bool	TryAssignBumpScaleValue(RPR::FMaterialNode MaterialNode, FRPRMaterialNormalMap* Map);

			};

		}
	}
}

