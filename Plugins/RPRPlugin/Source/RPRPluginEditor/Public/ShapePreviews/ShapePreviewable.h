#pragma once

#include "GCObject.h"
#include "UObjectGlobals.h"
#include "ShapePreviewBase.h"

template<typename ShapeType>
class FShapePreviewable : public FGCObject
{
public:

	FShapePreviewable()
	{
		static_assert(std::is_base_of<UShapePreviewBase, ShapeType>::value, "ShapeType of FShapePreviewable<ShapeType> can only inherit from UShapePreviewBase");
	}

	ShapeType*	GetShape()
	{
		if (Shape == nullptr)
		{
			Shape = NewObject<ShapeType>();
			Shape->SetFlags(RF_Transactional);
		}

		return (Shape);
	}

	void		ReleaseShape()
	{
		if (Shape != nullptr)
		{
			Shape->DestroyComponent();
			Shape = nullptr;
		}
	}

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		if (Shape != nullptr)
		{
			Collector.AddReferencedObject(Shape);
		}
	}

private:

	ShapeType*	Shape;

};
