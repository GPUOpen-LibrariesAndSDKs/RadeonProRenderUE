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
#pragma once

#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"
#include "Material/MaterialContext.h"
#include "ImageManager/RPRImageManager.h"

struct FRPRUberMaterialParameters;
struct FRPRUberMaterialParameterBase;
class URPRMaterial;

namespace RPRX
{
	namespace MaterialParameter
	{
		struct FArgs
		{
			const FRPRUberMaterialParameters& Parameters;
			const UProperty* Property;
			const URPRMaterial* OwnerMaterial;
			RPR::FMaterialContext& MaterialContext;
			FMaterial& Material;
			RPR::FImageManagerPtr ImageManager;

			FArgs(const FRPRUberMaterialParameters& InParameters, 
				const UProperty* InProperty, 
				RPR::FImageManagerPtr InImageManager,
				const URPRMaterial* OwnerMaterial,
				RPR::FMaterialContext& InMaterialContext, 
				FMaterial& InMaterial);

			template<typename ParameterType>
			const ParameterType*	GetDirectParameter();

            const FRPRUberMaterialParameterBase*	GetMaterialParameterBase() const;
            FRPRUberMaterialParameterBase*	        GetMaterialParameterBase();

			uint32		GetRprxParam() const;
            bool		CanUseParam() const;
            bool        HasCustomParameterApplier() const;
		};

		template<typename ParameterType>
		const ParameterType* FArgs::GetDirectParameter()
		{
			return (Property->ContainerPtrToValuePtr<ParameterType>(&Parameters));
		}
	}
}
