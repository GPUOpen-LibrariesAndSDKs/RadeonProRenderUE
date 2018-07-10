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
#include "Material/RPRMaterialBuilder.h"
#include "Helpers/RPRHelpers.h"
#include "Enums/RPREnums.h"
#include "Scene/RPRScene.h"
#include "Helpers/RPRXHelpers.h"
#include "Helpers/RPRErrorsHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialBuilder, Log, All)

namespace RPR
{

	FMaterialBuilder::FMaterialBuilder(ARPRScene* InScene)
		: Scene(InScene)
	{}

	void FMaterialBuilder::AddReferencedObjects(FReferenceCollector& Collector)
	{
		Collector.AddReferencedObject(Scene);
	}

	FResult FMaterialBuilder::BindMaterialRawDatasToShape(uint32 MaterialType, FMaterialRawDatas MaterialRawDatas, FShape& Shape)
	{
		FResult status = RPR_SUCCESS;

		switch (MaterialType)
		{
		
		case EMaterialType::Material:
		{
			status = SetMaterialOnShape(Shape, MaterialRawDatas);
		}
		break;

		case EMaterialType::MaterialX:
		{
			status = AttachMaterialToShape(Shape, MaterialRawDatas);
			if (IsResultSuccess(status))
			{
				CommitMaterial(MaterialType, MaterialRawDatas);
			}
		}
		break;

		default:
		{
			status = RPR_ERROR_UNSUPPORTED;
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Unknown material type"));
		}
		break;
		
		}

		if (status != RPR_SUCCESS)
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
		}

		return (status);
	}

	RPR::FResult FMaterialBuilder::CommitMaterial(uint32 MaterialType, FMaterialRawDatas MaterialRawDatas)
	{
		if (MaterialType == EMaterialType::MaterialX)
		{
			return (CommitMaterialX(MaterialRawDatas));
		}
		return (RPR_SUCCESS);
	}

	FResult FMaterialBuilder::SetMaterialOnShape(FShape& Shape, FMaterialRawDatas MaterialRawDatas)
	{
		FResult status = rprShapeSetMaterial(Shape, reinterpret_cast<RPRX::FMaterial>(MaterialRawDatas));

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't set the material on the shape"));
			RPR::Error::LogLastError(Scene->m_RprContext);
		}

		return (status);
	}

	FResult FMaterialBuilder::AttachMaterialToShape(FShape& Shape, FMaterialRawDatas MaterialRawDatas)
	{
		FResult status = RPRX::ShapeAttachMaterial(Scene->m_RprSupportCtx, Shape, reinterpret_cast<RPRX::FMaterial>(MaterialRawDatas));

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
			RPR::Error::LogLastError(Scene->m_RprContext);
		}

		return (status);
	}

	FResult FMaterialBuilder::CommitMaterialX(FMaterialRawDatas MaterialRawDatas)
	{		
		FResult status = RPRX::MaterialCommit(Scene->m_RprSupportCtx, reinterpret_cast<RPRX::FMaterial>(MaterialRawDatas));

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't commit RPR X material"));
			RPR::Error::LogLastError(Scene->m_RprContext);
		}

		return (status);
	}

}
