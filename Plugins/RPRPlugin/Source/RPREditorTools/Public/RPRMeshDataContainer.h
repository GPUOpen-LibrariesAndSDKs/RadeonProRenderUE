#pragma once
#include "Array.h"
#include "SharedPointer.h"
#include "RPRMeshData.h"
#include "Engine/StaticMesh.h"

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

	FRPRMeshDataPtr	FindByPreview(class URPRMeshPreviewComponent* PreviewMeshComponent);

	TArray<UStaticMesh*>	GetStaticMeshes() const;
	TArray<URPRMeshPreviewComponent*>	GetMeshPreviews() const;

	FORCEINLINE FRPRMeshDataPtr&		operator[](int32 index) { return (MeshDatas[index]); }
	FORCEINLINE const FRPRMeshDataPtr&	operator[](int32 index) const { return (MeshDatas[index]); }

private:

	TArray<FRPRMeshDataPtr>	MeshDatas;

};
