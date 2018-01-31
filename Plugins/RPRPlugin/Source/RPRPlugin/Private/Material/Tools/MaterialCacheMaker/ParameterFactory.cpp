#include "ParameterFactory.h"
#include "MaterialParameter.h"
#include "UberMaterialPropertyHelper.h"

namespace RPRX
{
	namespace MaterialParameter
	{
		TMap<FName, FFactory::FParameterCreator> FFactory::Map;

		void FFactory::InitializeMap()
		{
#define REGISTER_PARAMETER_CHECK_CLASS(ClassName, ParameterSetterClass)			\
			static_assert(TIsClass<ClassName>::Value, "Class doesn't exist!");	\
			RegisterParameterSetter<ParameterSetterClass>(#ClassName);

			REGISTER_PARAMETER_CHECK_CLASS(FRPRMaterialMap, FMaterialMapParameterSetter);
			REGISTER_PARAMETER_CHECK_CLASS(FRPRMaterialMapChannel1, FMaterialMapChannel1ParameterSetter);
			RegisterParameterSetter<FUInt8ParameterSetter>(TNameOf<uint8>::GetName());
		}

		void FFactory::InitializeMapIfRequired()
		{
			if (Map.Num() == 0)
			{
				InitializeMap();
			}
		}

		TSharedPtr<IMaterialParameter> FFactory::Create(UProperty* Property)
		{
			InitializeMapIfRequired();

			FString propertyNameType = FUberMaterialPropertyHelper::GetPropertyTypeName(Property);

			FParameterCreator* creator = Map.Find(*propertyNameType);
			return (creator != nullptr ? (*creator)() : nullptr);
		}
	}
}
