#include "ParameterSetterFactory.h"
#include "MaterialCacheParameterSetter.h"

namespace RPR
{
	namespace MaterialCacheParameterSetter
	{
		TMap<FName, RPR::MaterialCacheParameterSetter::FFactory::FMaterialCacheParameterSetterCreate> FFactory::Map;

		void FFactory::InitializeMap()
		{
			RegisterParameterSetter<FMaterialMapParameterSetter>("FRPRMaterialMap");
		}

		void FFactory::InitializeMapIfRequired()
		{
			if (Map.Num() == 0)
			{
				InitializeMap();
			}
		}

		TSharedPtr<IMaterialCacheParameterSetter> FFactory::Create(UProperty* Property)
		{
			InitializeMapIfRequired();

			FString propertyNameType = Property->GetCPPType();
			FMaterialCacheParameterSetterCreate* creator = Map.Find(*propertyNameType);
			return (creator != nullptr ? (*creator)() : nullptr);
		}

	}
}
