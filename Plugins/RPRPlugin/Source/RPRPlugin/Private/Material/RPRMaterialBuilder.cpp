#include "RPRMaterialBuilder.h"
#include "RPRHelpers.h"
#include "RPREnums.h"
#include "RPRScene.h"

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
				status = CommitMaterialX(MaterialRawDatas);
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

	FResult FMaterialBuilder::SetMaterialOnShape(FShape& Shape, FMaterialRawDatas MaterialRawDatas)
	{
		FResult status = rprShapeSetMaterial(Shape, reinterpret_cast<FMaterial>(MaterialRawDatas));

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't set the material on the shape"));
		}

		return (status);
	}

	FResult FMaterialBuilder::AttachMaterialToShape(FShape& Shape, FMaterialRawDatas MaterialRawDatas)
	{
		FResult status = rprxShapeAttachMaterial(Scene->m_RprSupportCtx, Shape, reinterpret_cast<FMaterial>(MaterialRawDatas));

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't assign RPR material to the RPR shape"));
		}

		return (status);
	}

	FResult FMaterialBuilder::CommitMaterialX(FMaterialRawDatas MaterialRawDatas)
	{
		
		FResult status = rprxMaterialCommit(Scene->m_RprSupportCtx, reinterpret_cast<FMaterial>(MaterialRawDatas));

		if (IsResultFailed(status))
		{
			UE_LOG(LogRPRMaterialBuilder, Warning, TEXT("Couldn't commit RPR X material"));
		}

		return (status);
	}

}