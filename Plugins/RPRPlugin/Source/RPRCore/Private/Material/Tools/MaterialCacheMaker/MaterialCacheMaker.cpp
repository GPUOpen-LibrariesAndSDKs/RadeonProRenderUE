/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "Material/Tools/MaterialCacheMaker/MaterialCacheMaker.h"
#include "Helpers/RPRHelpers.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/Tools/MaterialCacheMaker/Factory/ParameterFactory.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreModule.h"

namespace RPRX
{

	FMaterialCacheMaker::FMaterialCacheMaker(RPR::FMaterialContext InMaterialContext, const URPRMaterial* InRPRMaterial)
		: MaterialContext(InMaterialContext)
		, RPRMaterial(InRPRMaterial)
	{}

	bool FMaterialCacheMaker::CacheUberMaterial(RPRX::FMaterial& OutMaterial)
	{
		return 
			RPR::IsResultSuccess(RPRX::FMaterialHelpers::CreateMaterial(MaterialContext.RPRXContext, EMaterialType::Uber, OutMaterial)) && 
			UpdateUberMaterialParameters(OutMaterial);
	}

	bool	FMaterialCacheMaker::UpdateUberMaterialParameters(RPRX::FMaterial& InOutMaterial)
	{
		return RPR::IsResultSuccess(
			BrowseUberMaterialParameters(
				FUberMaterialParametersPropertyVisitor::CreateRaw(this, &FMaterialCacheMaker::ApplyUberMaterialParameter),
				InOutMaterial
			));
	}

	RPR::FResult FMaterialCacheMaker::BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, 
																				FMaterial& OutMaterial)
	{
		const FRPRUberMaterialParameters& uberMaterialParameters = RPRMaterial->MaterialParameters;
		UScriptStruct* parametersStruct = FRPRUberMaterialParameters::StaticStruct();
		RPR::FResult result = RPR_SUCCESS;

		UProperty* currentProperty = parametersStruct->PropertyLink;
		while (currentProperty != nullptr)
		{
			result = Visitor.Execute(uberMaterialParameters, parametersStruct, currentProperty, OutMaterial);
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
																		FMaterial& InOutMaterial)
	{
		RPR::FResult result = RPR_SUCCESS;

		RPR::FImageManagerPtr imageManager = IRPRCore::GetResources()->GetRPRImageManager();

		RPRX::MaterialParameter::FArgs materialCacheParametersSetterArgs(
			InParameters,
			InParameterProperty,
			imageManager,
			RPRMaterial,
			MaterialContext,
			InOutMaterial
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
