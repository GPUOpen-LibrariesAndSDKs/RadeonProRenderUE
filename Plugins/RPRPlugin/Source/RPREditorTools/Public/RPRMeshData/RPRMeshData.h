#pragma once
#include "Delegate.h"
#include "DelegateCombinations.h"
#include "Engine/StaticMesh.h"
#include "RPRMeshSection.h"
#include "RawMesh.h"

DECLARE_STATS_GROUP(TEXT("RPRMeshData"), STATGROUP_RPRMeshData, STATCAT_Advanced)

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

	/* Should be called if the number of sections has changed */
	void	RebuildSections();

	void	HighlightSection(int32 SectionIndex, bool bHighlight);

	FRPRMeshSection&		GetMeshSection(int32 Index);
	const FRPRMeshSection&	GetMeshSection(int32 Index) const;
	int32	GetNumSections() const;
	int32	CountNumSections() const;
	bool	HasAtLeastOneSectionSelected() const;

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
	bool				HasMeshChangesNotCommitted() const;

	void	DumpUV(int32 UVChannel);

private:

	void	UpdateAllBarycenters();
	void	UpdateBarycenter(int32 UVChannel);
	void	UpdatePreviewSectionHighlights();
	
private:
	
	TWeakObjectPtr<UStaticMesh> StaticMesh;

	/* The number of sections should match the number of sections in the static mesh */
	TArray<FRPRMeshSection> Sections;

	FRawMesh RawMesh;

	TArray<FVector2D, TInlineAllocator<MAX_MESH_TEXTURE_COORDS>> Barycenters;

	TWeakObjectPtr<class URPRStaticMeshPreviewComponent> Preview;

	bool bHasMeshChangesNotCommitted;

};

typedef TSharedPtr<FRPRMeshData> FRPRMeshDataPtr;