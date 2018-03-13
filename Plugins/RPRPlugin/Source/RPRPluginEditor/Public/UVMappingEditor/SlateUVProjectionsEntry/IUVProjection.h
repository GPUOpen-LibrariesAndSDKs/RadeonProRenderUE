#pragma once

#include "SharedPointer.h"
#include "SWidget.h"

DECLARE_DELEGATE(FOnProjectionApplied)

class IUVProjection
{
public:
	/* Called when the creation is done */
	virtual void	FinalizeCreation() = 0;
	/* Called when the widget is displayed */
	virtual void	OnUVProjectionDisplayed() = 0;
	/*  Called when the widget is hidden */
	virtual void	OnUVProjectionHidden() = 0;

	virtual void					SetRPRStaticMeshEditor(TWeakPtr<class FRPRStaticMeshEditor> RPRStaticMeshEditor) = 0;
	virtual class UStaticMesh*		GetStaticMesh() const = 0;
	virtual TSharedRef<SWidget>		TakeWidget() = 0;
	virtual FOnProjectionApplied&	OnProjectionApplied() = 0;
};

typedef TSharedPtr<IUVProjection>	IUVProjectionPtr;