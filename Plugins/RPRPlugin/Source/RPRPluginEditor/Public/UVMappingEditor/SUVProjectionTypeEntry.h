#pragma once

#include "SharedPointer.h"
#include "SlateBrush.h"
#include "UVProjectionType.h"
#include "SCompoundWidget.h"
#include "DeclarativeSyntaxSupport.h"
#include "IUVProjection.h"
#include "GCObject.h"
#include "Engine/StaticMesh.h"
#include "SWidget.h"

class SUVProjectionTypeEntry : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionTypeEntry)
		: _ProjectionType(EUVProjectionType::Planar)
		, _ProjectionName()
		, _StaticMesh()
		, _Icon(nullptr)
	{}

	SLATE_ARGUMENT(EUVProjectionType, ProjectionType)
	SLATE_ARGUMENT(FText, ProjectionName)
	SLATE_ARGUMENT(UStaticMesh*, StaticMesh)
	SLATE_ARGUMENT(const FSlateBrush*, Icon)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	
	EUVProjectionType	GetProjectionType() const;
	IUVProjectionPtr	GetUVProjectionWidget() const;

private:

	EUVProjectionType	ProjectionType;
	IUVProjectionPtr	ProjectionWidget;
	UStaticMesh*		StaticMesh;

};

typedef TSharedPtr<SUVProjectionTypeEntry>	SUVProjectionTypeEntryPtr;