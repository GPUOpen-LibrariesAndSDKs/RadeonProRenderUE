#include "GLTFMaterialParameterSetterFactory.h"

#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Material/UberMaterialParameters/RPRMaterialEnum.h"
#include "Material/UberMaterialParameters/RPRMaterialBool.h"
#include "Material/UberMaterialParameters/RPRMaterialCoMChannel1.h"
#include "Material/UberMaterialParameters/RPRMaterialNormalMap.h"

#include "Factories/Setters/RPRMaterialMapSetter.h"
#include "Factories/Setters/RPRMaterialCoMSetter.h"
#include "Factories/Setters/RPRMaterialEnumSetter.h"
#include "Factories/Setters/RPRMaterialBoolSetter.h"
#include "Factories/Setters/RPRMaterialCoMChannel1Setter.h"
#include "Factories/Setters/RPRMaterialNormalMapSetter.h"

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
					ADD_CLASS_TO_FACTORY(FRPRMaterialEnum, FRPRMaterialEnumSetter);
					ADD_CLASS_TO_FACTORY(FRPRMaterialBool, FRPRMaterialBoolSetter);
					ADD_CLASS_TO_FACTORY(FRPRMaterialCoM, FRPRMaterialCoMSetter);
					ADD_CLASS_TO_FACTORY(FRPRMaterialNormalMap, FRPRMaterialNormalMapSetter);
					ADD_CLASS_TO_FACTORY(FRPRMaterialCoMChannel1, FRPRMaterialCoMChannel1Setter);
					ADD_CLASS_TO_FACTORY(FRPRMaterialMap, FRPRMaterialMapSetter);
				}
			#undef ADD_CLASS_TO_FACTORY
			}

		} // namespace Importer

	} // namespace GLTF

} // namespace RPR
