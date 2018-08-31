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
#include "Material/Tools/MaterialCacheMaker/ParameterArgs.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Assets/RPRMaterial.h"
#include "Helpers/RPRConstAway.h"

namespace RPRX
{
	namespace MaterialParameter
	{

		FArgs::FArgs(
			const FRPRUberMaterialParameters& InParameters, 
			const UProperty* InProperty, 
			RPR::FImageManagerPtr InImageManager,
			const URPRMaterial* InOwnerMaterial,
			RPR::FMaterialContext& InMaterialContext, 
			RPR::FRPRXMaterial& InMaterial)
			: Parameters(InParameters)
			, Property(InProperty)
			, OwnerMaterial(InOwnerMaterial)
			, MaterialContext(InMaterialContext)
			, Material(InMaterial)
			, ImageManager(InImageManager)
		{}

		const FRPRUberMaterialParameterBase* FArgs::GetMaterialParameterBase() const
		{
			return (FUberMaterialPropertyHelper::GetParameterBaseFromPropertyConst(&Parameters, Property));
		}

        FRPRUberMaterialParameterBase* FArgs::GetMaterialParameterBase()
        {
            const FArgs* thisConst = this;
            return (RPR::ConstRefAway(thisConst->GetMaterialParameterBase()));
        }

        uint32 FArgs::GetRprxParam() const
		{
			const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
			return (materialParameter->GetRprxParamType());
		}

		bool FArgs::CanUseParam() const
		{
			const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
			return (materialParameter->CanUseParameter());
		}

        bool FArgs::HasCustomParameterApplier() const
        {
            const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
            return (materialParameter->HasCustomParameterApplier());
        }

	}
}
