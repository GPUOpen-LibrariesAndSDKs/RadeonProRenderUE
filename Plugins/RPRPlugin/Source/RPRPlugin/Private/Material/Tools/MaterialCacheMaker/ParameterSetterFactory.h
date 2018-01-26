#pragma once

#include "UnrealType.h"

namespace RPR
{
	namespace MaterialCacheParameterSetter
	{

		class FFactory
		{

			typedef TSharedPtr<class IMaterialCacheParameterSetter>(*FMaterialCacheParameterSetterCreate)();

		public:

			static TSharedPtr<class IMaterialCacheParameterSetter>	Create(UProperty* Property);

			template<typename ParameterSetterType>
			static void	RegisterParameterSetter(const FName& PropertyTypeName);

		private:

			static void		InitializeMap();
			static void		InitializeMapIfRequired();
			
			template<typename ParameterSetterType>
			static TSharedPtr<IMaterialCacheParameterSetter>	InstantiateParameterType();

		private:

			static TMap<FName, FMaterialCacheParameterSetterCreate> Map;
		};

		template<typename ParameterSetterType>
		void	FFactory::RegisterParameterSetter(const FName& PropertyTypeName)
		{
			Map.Add(PropertyTypeName, &FFactory::InstantiateParameterType<ParameterSetterType>);
		}

		template<typename ParameterSetterType>
		static TSharedPtr<IMaterialCacheParameterSetter> FFactory::InstantiateParameterType()
		{
			return MakeShareable(new ParameterSetterType());
		}

	}
}