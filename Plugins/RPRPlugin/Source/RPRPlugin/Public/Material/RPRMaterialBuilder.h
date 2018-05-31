#pragma once

#include "RPRTypedefs.h"
#include "SRPRShape.h"
#include "GCObject.h"
#include "RPRScene.h"

namespace RPR
{
	/*
	* Helper to assign a RPR material to a RPR shape
	*/
	class FMaterialBuilder : public FGCObject
	{
	public:

		FMaterialBuilder(class ARPRScene* InScene);

		virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
		
		FResult	BindMaterialRawDatasToShape(uint32 MaterialType, FMaterialRawDatas MaterialRawDatas, FShape& Shape);
		FResult CommitMaterial(uint32 MaterialType, FMaterialRawDatas MaterialRawDatas);
		
	private:

		FResult	SetMaterialOnShape(FShape& Shape, FMaterialRawDatas MaterialRawDatas);
		FResult	AttachMaterialToShape(FShape& Shape, FMaterialRawDatas MaterialRawDatas);
		FResult	CommitMaterialX(FMaterialRawDatas MaterialRawDatas);

	private:

		ARPRScene*	Scene;

	};
}