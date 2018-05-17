#include "ParameterArgs.h"
#include "UberMaterialPropertyHelper.h"
#include "UberMaterialPropertyHelper.h"

namespace RPRX
{
	namespace MaterialParameter
	{

		FArgs::FArgs(const FRPRUberMaterialParameters& InParameters, const UProperty* InProperty, 
			RPR::FMaterialContext& InMaterialContext, FMaterial& InMaterial)
			: Parameters(InParameters)
			, Property(InProperty)
			, MaterialContext(InMaterialContext)
			, Material(InMaterial)
		{}

		const FRPRUberMaterialParameterBase* FArgs::GetMaterialParameterBase() const
		{
			return (FUberMaterialPropertyHelper::GetParameterBaseFromProperty(&Parameters, Property));
		}

		uint32 FArgs::GetRprxParam() const
		{
			const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
			return (materialParameter->GetRprxParam());
		}

	}
}