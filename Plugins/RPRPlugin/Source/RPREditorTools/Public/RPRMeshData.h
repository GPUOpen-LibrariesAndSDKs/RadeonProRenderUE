#pragma once
#include "Delegate.h"
#include "DelegateCombinations.h"
#include "Engine/StaticMesh.h"
#include "RawMesh.h"

DECLARE_MULTICAST_DELEGATE(FOnMeshDataChanged)

class RPREDITORTOOLS_API FRPRMeshData
{
public:

	FOnMeshDataChanged	OnPostStaticMeshMaterialChange;
	FOnMeshDataChanged	OnPostStaticMeshChange;
	FOnMeshDataChanged	OnPostRawMeshChange;

public:

	FRPRMeshData(UStaticMesh* InStaticMesh);

	void	AssignPreview(class URPRStaticMeshPreviewComponent* InPreviewMeshComponent);
	void	ApplyRawMeshDatas();

	void	NotifyRawMeshChanges();
	void	NotifyStaticMeshChanges();
	void	NotifyStaticMeshMaterialChanges();

	/* You should call NotifyRawMeshChange once you are done with your modification */
	FORCEINLINE FRawMesh& GetRawMesh() { return (RawMesh); }
	FORCEINLINE const FRawMesh&	GetRawMesh() const { return (RawMesh); }

	/* You should call NotifyStaticMeshChange once you are done with your modification */
	FORCEINLINE UStaticMesh* GetStaticMesh() { return (StaticMesh.Get()); }
	FORCEINLINE const UStaticMesh* GetStaticMesh() const { return (StaticMesh.Get()); }

	FORCEINLINE class URPRStaticMeshPreviewComponent* GetPreview() const { return (Preview.Get()); }
	FORCEINLINE TWeakObjectPtr<class URPRStaticMeshPreviewComponent> GetWeakPreview() const { return (Preview); }

	int32				GetNumUVChannelsUsed() const;
	const FVector2D&	GetUVBarycenter(int32 UVChannel = 0) const;

private:

	void	UpdateAllBarycenters();
	void	UpdateBarycenter(int32 UVChannel);
	
private:
	
	TWeakObjectPtr<UStaticMesh> StaticMesh;
	FRawMesh RawMesh;

	TArray<FVector2D, TInlineAllocator<MAX_MESH_TEXTURE_COORDS>> Barycenters;

	TWeakObjectPtr<class URPRStaticMeshPreviewComponent> Preview;

};

typedef TSharedPtr<FRPRMeshData> FRPRMeshDataPtr;