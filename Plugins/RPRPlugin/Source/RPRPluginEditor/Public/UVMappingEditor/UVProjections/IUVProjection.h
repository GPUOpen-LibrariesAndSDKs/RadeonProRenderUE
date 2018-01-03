#pragma once

#include "SharedPointer.h"
#include "SWidget.h"

class IUVProjection
{
public:
	virtual void					FinalizeCreation() = 0;
	virtual void					Release() = 0;
	virtual void					SetRPRStaticMeshEditor(TWeakPtr<class FRPRStaticMeshEditor> RPRStaticMeshEditor) = 0;
	virtual void					SetStaticMesh(class UStaticMesh* StaticMesh) = 0;
	virtual class UStaticMesh*		GetStaticMesh() const = 0;
	virtual TSharedRef<SWidget>		TakeWidget() = 0;
};

typedef TSharedPtr<IUVProjection>	IUVProjectionPtr;