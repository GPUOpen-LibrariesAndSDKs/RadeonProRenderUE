/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Material/Tools/MaterialCacheMaker/MaterialCacheMaker.h"
#include "Helpers/RPRHelpers.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/Tools/MaterialCacheMaker/Factory/ParameterFactory.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreModule.h"

namespace RPRX
{

	FMaterialCacheMaker::FMaterialCacheMaker(RPR::FMaterialContext InMaterialContext, const URPRMaterial* InRPRMaterial)
		: MaterialContext(InMaterialContext)
		, RPRMaterial(InRPRMaterial)
	{}

	RPR::FRPRXMaterialPtr FMaterialCacheMaker::CacheUberMaterial()
	{
		RPR::FRPRXMaterialPtr materialPtr = MakeShareable(new RPR::FRPRXMaterial(RPRMaterial));
		if (!UpdateUberMaterialParameters(materialPtr))
		{
			materialPtr.Reset();
			return nullptr;
		}

		return materialPtr;
	}

	bool FMaterialCacheMaker::UpdateUberMaterialParameters(RPR::FRPRXMaterialPtr Material)
	{
		bool bIsMaterialCorrectlyUpdated;

		FUberMaterialParametersPropertyVisitor visitor = FUberMaterialParametersPropertyVisitor::CreateRaw(this, &FMaterialCacheMaker::ApplyUberMaterialParameter);
		bIsMaterialCorrectlyUpdated = RPR::IsResultSuccess(BrowseUberMaterialParameters(visitor, Material));

		return bIsMaterialCorrectlyUpdated;
	}

	RPR::FResult FMaterialCacheMaker::BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor,
																	RPR::FRPRXMaterialPtr Material)
	{
		const FRPRUberMaterialParameters& uberMaterialParameters = RPRMaterial->MaterialParameters;
		UScriptStruct* parametersStruct = FRPRUberMaterialParameters::StaticStruct();
		RPR::FResult result = RPR_SUCCESS;

		UProperty* currentProperty = parametersStruct->PropertyLink;
		while (currentProperty != nullptr)
		{
			result = Visitor.Execute(uberMaterialParameters, parametersStruct, currentProperty, Material);
			if (RPR::IsResultFailed(result))
			{
				return (result);
			}

			currentProperty = currentProperty->PropertyLinkNext;
		}
		return (result);
	}

	RPR::FResult FMaterialCacheMaker::ApplyUberMaterialParameter(const FRPRUberMaterialParameters& InParameters,
																		UScriptStruct* InParametersStruct,
																		UProperty* InParameterProperty,
																		RPR::FRPRXMaterialPtr Material)
	{
		RPR::FResult result = RPR_SUCCESS;

		RPR::FImageManagerPtr imageManager = IRPRCore::GetResources()->GetRPRImageManager();

		RPRX::MaterialParameter::FArgs materialCacheParametersSetterArgs(
			InParameters,
			InParameterProperty,
			imageManager,
			RPRMaterial,
			MaterialContext,
			Material
		);

		if (materialCacheParametersSetterArgs.CanUseParam())
		{
            if (materialCacheParametersSetterArgs.HasCustomParameterApplier())
            {
				UE_LOG(LogRPRCore_Steps, VeryVerbose, TEXT("[%s] %s -> Parameter use custom application"), *RPRMaterial->GetName(), *InParameterProperty->GetName());

                FRPRUberMaterialParameterBase* materialParameter = materialCacheParametersSetterArgs.GetMaterialParameterBase();
                materialParameter->ApplyParameter(materialCacheParametersSetterArgs);
            }
            else
            {
                TSharedPtr<RPRX::IMaterialParameter> mapSetter =
                    RPRX::MaterialParameter::FFactory::Create(InParameterProperty);

                if (mapSetter.IsValid())
                {
					UE_LOG(LogRPRCore_Steps, VeryVerbose, TEXT("[%s] %s -> Parameter use standard application"), *RPRMaterial->GetName(), *InParameterProperty->GetName());

                    mapSetter->ApplyParameterX(materialCacheParametersSetterArgs);
                }
            }
		}
		else
		{
			UE_LOG(LogRPRCore_Steps, VeryVerbose, TEXT("[%s] %s -> Parameter not used"), *RPRMaterial->GetName(), *InParameterProperty->GetName());
		}

		return (result);
	}
}
