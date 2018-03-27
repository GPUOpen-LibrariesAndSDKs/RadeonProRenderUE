#pragma once
#include "Delegate.h"
#include "DelegateCombinations.h"
#include "Engine/StaticMesh.h"
#include "RawMesh.h"

DECLARE_MULTICAST_DELEGATE(FOnMeshDataChanged)

class RPREDITORTOOLS_API FRPRMeshData
{
public:

	FOnMeshDataChanged	OnPostStaticMeshChange;
	FOnMeshDataChanged	OnPostRawMeshChange;

public:

	FRPRMeshData(UStaticMesh* InStaticMesh);

	void	AssignPreview(class URPRMeshPreviewComponent* InPreviewMeshComponent);
	void	ApplyRawMeshDatas();
	
	void	NotifyRawMeshChanges();
	void	NotifyStaticMeshChanges();

	/* You should call NotifyRawMeshChange once you are done with your modification */
	FORCEINLINE FRawMesh& GetRawMesh() { return (RawMesh); }
	FORCEINLINE const FRawMesh&	GetRawMesh() const { return (RawMesh); }

	/* You should call NotifyStaticMeshChange once you are done with your modification */
	FORCEINLINE UStaticMesh* GetStaticMesh() { return (StaticMesh.Get()); }
	FORCEINLINE const UStaticMesh* GetStaticMesh() const { return (StaticMesh.Get()); }

	FORCEINLINE class URPRMeshPreviewComponent* GetPreview() const { return (Preview.Get()); }
	FORCEINLINE TWeakObjectPtr<class URPRMeshPreviewComponent> GetWeakPreview() const { return (Preview); }
	
private:
	
	TWeakObjectPtr<UStaticMesh> StaticMesh;
	FRawMesh RawMesh;

	TWeakObjectPtr<class URPRMeshPreviewComponent> Preview;

};

typedef TSharedPtr<FRPRMeshData> FRPRMeshDataPtr;