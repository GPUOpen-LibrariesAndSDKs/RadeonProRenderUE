#pragma once
#include "Array.h"
#include "SharedPointer.h"
#include "RPRMeshData.h"
#include "Engine/StaticMesh.h"
#include "DelegateCombinations.h"

DECLARE_DELEGATE_TwoParams(FOnEachUV, int32 /* MeshIndex */, FVector2D& /* UV */)

class RPREDITORTOOLS_API FRPRMeshDataContainer
{
public:

	void	Add(FRPRMeshDataPtr MeshDataPtr);
	void	Reserve(int32 Capacity);
	void	Empty(int32 Capacity = 0);
	int32	Num() const;
	FRPRMeshDataPtr	Last() const;

	void	AppendFromStaticMeshes(const TArray<class UStaticMesh*>& StaticMeshes);
	void	RemoveInvalidStaticMeshes();
	void	GetAllUV(TArray<int32>& MeshUVStartIndexes, TArray<FVector2D>& UVs, int32 UVChannel = 0) const;
	FVector2D	GetUVBarycenter(int32 UVChannel = 0) const;

	void	Broadcast_NotifyRawMeshChanges();
	void	Broadcast_ApplyRawMeshDatas();

	FRPRMeshDataPtr	FindByPreview(class URPRMeshPreviewComponent* PreviewMeshComponent);

	TArray<UStaticMesh*>	GetStaticMeshes() const;
	TArray<URPRMeshPreviewComponent*>	GetMeshPreviews() const;

	int32	GetMaxUVChannelUsedIndex() const;
	void	GetMeshesBounds(FVector& OutCenter, FVector& OutExtents) const;
	void	GetMeshesBoxSphereBounds(FBoxSphereBounds& OutBounds) const;

	FORCEINLINE FRPRMeshDataPtr&		operator[](int32 index) { return (MeshDatas[index]); }
	FORCEINLINE const FRPRMeshDataPtr&	operator[](int32 index) const { return (MeshDatas[index]); }

	void	OnEachUV(int32 UVChannel, FOnEachUV OnEachUVPredicate);

private:

	TArray<FRPRMeshDataPtr>	MeshDatas;

};

typedef TSharedPtr<FRPRMeshDataContainer> FRPRMeshDataContainerPtr;
typedef TWeakPtr<FRPRMeshDataContainer> FRPRMeshDataContainerWkPtr;