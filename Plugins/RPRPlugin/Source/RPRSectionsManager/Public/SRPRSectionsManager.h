#pragma once
#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "IDetailsView.h"
#include "RPRMeshDataContainer.h"

DECLARE_DELEGATE_RetVal(FRPRMeshDataContainerPtr, FGetRPRMeshDatas)

class RPRSECTIONSMANAGER_API SRPRSectionsManager : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SRPRSectionsManager) {}
		SLATE_EVENT(FGetRPRMeshDatas, GetRPRMeshDatas)
		SLATE_EVENT(FSimpleDelegate, OnMaterialChanged)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

private:

	void	InitializeModeDetails();
	void	InitializeMeshesSectionsDetails();
	void	OnMaterialChanged(UStaticMesh* StaticMesh);
	FRPRMeshDataPtr GetRPRMeshData(UStaticMesh* StaticMesh);

	TSharedRef<IDetailCustomization> MakeStaticMeshDetails();

private:

	TSharedPtr<IDetailsView> RPRModeDetails;
	TSharedPtr<IDetailsView> MeshDetails;

	FGetRPRMeshDatas GetRPRMeshDatas;
	FSimpleDelegate MaterialChanged;

};
