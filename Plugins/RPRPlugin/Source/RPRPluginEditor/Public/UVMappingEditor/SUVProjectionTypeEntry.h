#pragma once

#include "SharedPointer.h"
#include "SlateBrush.h"
#include "UVProjectionType.h"
#include "SCompoundWidget.h"
#include "DeclarativeSyntaxSupport.h"
#include "IUVProjection.h"
#include "GCObject.h"
#include "Engine/StaticMesh.h"
#include "RPRStaticMeshEditor.h"
#include "SWidget.h"

DECLARE_DELEGATE(FOnProjectionApplied)

class SUVProjectionTypeEntry : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionTypeEntry)
		: _ProjectionType(EUVProjectionType::Planar)
		, _RPRStaticMeshEditor()
		, _ProjectionName()
		, _StaticMesh()
		, _Icon(nullptr)
	{}

	SLATE_ARGUMENT(EUVProjectionType, ProjectionType)
		SLATE_ARGUMENT(FRPRStaticMeshEditorPtr, RPRStaticMeshEditor)
		SLATE_ARGUMENT(FText, ProjectionName)
		SLATE_ARGUMENT(UStaticMesh*, StaticMesh)
		SLATE_ARGUMENT(const FSlateBrush*, Icon)
		SLATE_EVENT(FOnProjectionApplied, OnProjectionApplied)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	EUVProjectionType	GetProjectionType() const;
	IUVProjectionPtr	GetUVProjectionWidget() const;


private:

	void	NotifyProjectionApplied();


private:

	EUVProjectionType		ProjectionType;
	IUVProjectionPtr		ProjectionWidget;
	UStaticMesh*			StaticMesh;
	FOnProjectionApplied	OnProjectionApplied;

};

typedef TSharedPtr<SUVProjectionTypeEntry>	SUVProjectionTypeEntryPtr;