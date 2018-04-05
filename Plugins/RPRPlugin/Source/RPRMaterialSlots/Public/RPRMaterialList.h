#pragma once
#include "IDetailCustomNodeBuilder.h"
#include "RPRMeshDataContainer.h"

DECLARE_DELEGATE_RetVal(FRPRMeshDataContainerPtr, FGetStaticMeshes)

class FRPRMaterialList : public IDetailCustomNodeBuilder
{
public:

	struct FDelegates
	{
		FGetStaticMeshes GetStaticMeshes;
	};

public:
	
	FRPRMaterialList(class IDetailLayoutBuilder& LayoutBuilder, const FDelegates& Delegates);

	virtual void SetOnRebuildChildren(FSimpleDelegate InOnRegenerateChildren) override { OnRebuildChildren = InOnRegenerateChildren; }
	virtual void GenerateHeaderRowContent(FDetailWidgetRow& NodeRow) override;
	virtual void GenerateChildContent(IDetailChildrenBuilder& ChildrenBuilder) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool RequiresTick() const override;
	virtual bool InitiallyCollapsed() const override;
	virtual FName GetName() const override;

private:

	IDetailLayoutBuilder& LayoutBuilder;
	FDelegates	Delegates;

	/** Called to rebuild the children of the detail tree */
	FSimpleDelegate OnRebuildChildren;

};