#include "RPRMaterialList.h"
#include "STextBlock.h"
#include "DetailWidgetRow.h"
#include "DetailLayoutBuilder.h"

#define LOCTEXT_NAMESPACE "RPRMaterialList"

FRPRMaterialList::FRPRMaterialList(IDetailLayoutBuilder& InLayoutBuilder, const FDelegates& InDelegates)
	: LayoutBuilder(InLayoutBuilder)
	, Delegates(InDelegates)
{}

void FRPRMaterialList::GenerateHeaderRowContent(FDetailWidgetRow& NodeRow)
{
	NodeRow.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("MaterialsHeaderTitle", "Materials"))
			.Font(IDetailLayoutBuilder::GetDetailFont())
		];
}

void FRPRMaterialList::GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder)
{
	
}

void FRPRMaterialList::Tick(float DeltaTime)
{
	
}

bool FRPRMaterialList::RequiresTick() const
{
	return (false);
}

bool FRPRMaterialList::InitiallyCollapsed() const
{
	return (false);
}

FName FRPRMaterialList::GetName() const
{
	return (TEXT("RPRMaterialList"));
}

#undef LOCTEXT_NAMESPACE