#include "ParameterFactory.h"
#include "UberMaterialPropertyHelper.h"

#include "MaterialMapChannel1ParameterSetter.h"
#include "MaterialMapParameterSetter.h"
#include "MaterialEnumParameterSetter.h"
#include "MaterialBoolParameterSetter.h"

namespace RPRX
{
	namespace MaterialParameter
	{
		TMap<FName, FFactory::FParameterCreator> FFactory::Map;

		void FFactory::InitializeMap()
		{
#define ADD_TO_FACTORY_CHECK_CLASS(ClassName, ParameterSetterClass)			\
			static_assert(TIsClass<ClassName>::Value, "Class doesn't exist!");	\
			RegisterParameterSetter<ParameterSetterClass>(#ClassName);

			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMapChannel1, FMaterialMapChannel1ParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMap, FMaterialMapParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FMaterialEnumParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialBool, FMaterialBoolParameterSetter);
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
