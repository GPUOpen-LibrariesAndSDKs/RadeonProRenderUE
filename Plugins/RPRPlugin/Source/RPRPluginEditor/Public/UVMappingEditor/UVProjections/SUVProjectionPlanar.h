#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "IUVProjection.h"
#include "SharedPointer.h"
#include "SCompoundWidget.h"

class SUVProjectionPlanar : public SCompoundWidget, public IUVProjection
{
public:
	SLATE_BEGIN_ARGS(SUVProjectionPlanar)
	{}

	SLATE_END_ARGS()

	SUVProjectionPlanar() {}

	void Construct(const FArguments& InArgs);

	virtual TSharedRef<SWidget>	TakeWidget();

};