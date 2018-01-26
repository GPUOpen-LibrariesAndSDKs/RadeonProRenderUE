#pragma once

#include "UnrealType.h"
#include "MaterialParameter.h"

namespace RPRX
{
	namespace MaterialParameter
	{
		class FFactory
		{

			typedef TSharedPtr<IMaterialParameter>(*FParameterCreator)();

		public:

			static TSharedPtr<IMaterialParameter>	Create(UProperty* Property);

			template<typename ParameterSetterType>
			static void		RegisterParameterSetter(const FName& PropertyTypeName);

		private:

			static void		InitializeMap();
			static void		InitializeMapIfRequired();

			template<typename ParameterSetterType>
			static TSharedPtr<IMaterialParameter>	InstantiateParameterType();

		private:

			static TMap<FName, FParameterCreator> Map;
		};

		//////////////////////////////
		/* Template implementations */
		//////////////////////////////

		template<typename ParameterSetterType>
		void	FFactory::RegisterParameterSetter(const FName& PropertyTypeName)
		{
			Map.Add(PropertyTypeName, &FFactory::InstantiateParameterType<ParameterSetterType>);
		}

		template<typename ParameterSetterType>
		static TSharedPtr<IMaterialParameter> FFactory::InstantiateParameterType()
		{
			return MakeShareable(new ParameterSetterType());
		}
	}
}