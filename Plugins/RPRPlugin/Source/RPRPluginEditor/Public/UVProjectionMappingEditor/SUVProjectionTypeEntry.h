#pragma once

#include "SCompoundWidget.h"
#include "IUVProjectionModule.h"
#include "SharedPointer.h"
#include "DeclarativeSyntaxSupport.h"

class IUVProjectionModule;

class SUVProjectionTypeEntry : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionTypeEntry) 
		: _UVProjectionModulePtr()
	{}
		SLATE_ARGUMENT(IUVProjectionModule*, UVProjectionModulePtr)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	IUVProjectionModule* GetUVProjectionModule() const;

private:

	IUVProjectionModule* UVProjectionModulePtr;

};

typedef TSharedPtr<SUVProjectionTypeEntry>	SUVProjectionTypeEntryPtr;