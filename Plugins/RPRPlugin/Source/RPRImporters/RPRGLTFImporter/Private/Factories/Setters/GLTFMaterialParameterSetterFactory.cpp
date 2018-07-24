#include "GLTFMaterialParameterSetterFactory.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Factories/Setters/RPRMaterialMapSetter.h"
#include "Factories/Setters/RPRMaterialCoMSetter.h"

DECLARE_LOG_CATEGORY_CLASS(LogGLTFMaterialParameterSetterFactory, Log, All)

namespace RPR
{
	namespace GLTF
	{
		namespace Importer
		{
			TMap<UStruct*, FGLTFMaterialParameterSetterFactory::FCreator> FGLTFMaterialParameterSetterFactory::Factory;

			IRPRMaterialParameterSetterPtr FGLTFMaterialParameterSetterFactory::CreateMaterialParameterSetter(UStruct* ParameterClass)
			{
				InitializeFactoryIFN();

				FCreator* creator = Factory.Find(ParameterClass);
				if (creator == nullptr)
				{
					UE_LOG(LogGLTFMaterialParameterSetterFactory, Error, TEXT("Unsupported class : %s"), *ParameterClass->GetName());
					return (nullptr);
				}

				return ((*creator)());
			}

			void FGLTFMaterialParameterSetterFactory::InitializeFactoryIFN()
			{
				if (Factory.Num() > 0)
				{
					return;
				}

			#define ADD_CLASS_TO_FACTORY(ParameterType, ParameterSetterClass) \
				Factory.Add(ParameterType::StaticStruct(), [] () { return MakeShareable(new ParameterSetterClass); });
				{
					ADD_CLASS_TO_FACTORY(FRPRMaterialCoM, FRPRMaterialCoMSetter);
					ADD_CLASS_TO_FACTORY(FRPRMaterialMap, FRPRMaterialMapSetter);
				}
			#undef ADD_CLASS_TO_FACTORY
			}

		} // namespace Importer

	} // namespace GLTF

} // namespace RPR
