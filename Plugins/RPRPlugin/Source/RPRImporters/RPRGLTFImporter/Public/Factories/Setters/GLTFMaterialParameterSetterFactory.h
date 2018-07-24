#pragma once
#include "Factories/Setters/IRPRMaterialParameterSetter.h"
#include "UObject/Class.h"
#include "Containers/Map.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{

			class FGLTFMaterialParameterSetterFactory
			{
			public:
				static IRPRMaterialParameterSetterPtr	CreateMaterialParameterSetter(UStruct* ParameterClass);

			private:
				static void InitializeFactoryIFN();

			private:
				using FCreator = TFunction<IRPRMaterialParameterSetterPtr()>;
				static TMap<UStruct*, FCreator> Factory;
			};

		} // namespace Importer

	} // namespace GLTF

} // namespace RPR