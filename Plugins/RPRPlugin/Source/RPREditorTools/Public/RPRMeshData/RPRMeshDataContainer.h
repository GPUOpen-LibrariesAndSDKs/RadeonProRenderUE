/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#pragma once
#include "Array.h"
#include "SharedPointer.h"
#include "RPRMeshData.h"
#include "Engine/StaticMesh.h"
#include "AssertionMacros.h"
#include "DelegateCombinations.h"

// Verify function exists to it will tell if the function name has changed	

// Use to reroute function calls to RPRMeshData
#define DECLARE_BROADCAST(FunctionName) \
	void	Broadcast_##FunctionName() \
	{ \
		FName t = GET_FUNCTION_NAME_CHECKED(FRPRMeshData, FunctionName); \
		OnEachMeshData([](FRPRMeshDataPtr MeshData) { \
			MeshData->##FunctionName(); \
		}); \
	}

DECLARE_DELEGATE_TwoParams(FOnEachUV, int32 /* MeshIndex */, FVector2D& /* UV */)

class RPREDITORTOOLS_API FRPRMeshDataContainer
{
public:

	void	Add(FRPRMeshDataPtr MeshDataPtr);
	void	Reserve(int32 Capacity);
	void	Empty(int32 Capacity = 0);
	int32	Num() const;
	FRPRMeshDataPtr	Last() const;
	int32	IndexOf(FRPRMeshDataPtr MeshDataPtr) const;

	void	AppendFromStaticMeshes(const TArray<class UStaticMesh*>& StaticMeshes);
	void	RemoveInvalidStaticMeshes();
	void	GetAllUV(TArray<int32>& MeshUVStartIndexes, TArray<FVector2D>& UVs, int32 UVChannel = 0) const;
	FVector2D	GetUVBarycenter(int32 UVChannel = 0) const;
	bool	HasMeshesChangesNotCommitted() const;
	int32	CountNumSelectedSections() const;
	bool	FindFirstSelectedSection(FRPRMeshDataPtr& OutMeshData, int32& OutSectionIndex) const;

	DECLARE_BROADCAST(NotifyRawMeshChanges);
	DECLARE_BROADCAST(NotifyStaticMeshChanges);
	DECLARE_BROADCAST(NotifyStaticMeshMaterialChanges);
	DECLARE_BROADCAST(ApplyRawMeshDatas);

	FRPRMeshDataPtr	FindByPreview(class URPRStaticMeshPreviewComponent* PreviewMeshComponent);
	FRPRMeshDataPtr	FindByStaticMesh(class UStaticMesh* StaticMesh);

	TArray<UStaticMesh*>	GetStaticMeshes() const;
	TArray<UObject*>		GetStaticMeshesAsObjects() const;
	TArray<URPRStaticMeshPreviewComponent*>	GetMeshPreviews() const;

	int32	GetMaxUVChannelUsedIndex() const;
	void	GetMeshesBounds(FVector& OutCenter, FVector& OutExtents) const;
	void	GetMeshesBoxSphereBounds(FBoxSphereBounds& OutBounds) const;

	FORCEINLINE FRPRMeshDataPtr&		operator[](int32 index) { return (MeshDatas[index]); }
	FORCEINLINE const FRPRMeshDataPtr&	operator[](int32 index) const { return (MeshDatas[index]); }

	FORCEINLINE FRPRMeshDataPtr&		Get(int32 Index) { return (*this)[Index]; }
	FORCEINLINE const FRPRMeshDataPtr&	Get(int32 Index) const { return (*this)[Index]; }

	void	OnEachUV(int32 UVChannel, FOnEachUV OnEachUVPredicate);

	template<typename TPredicate>
	void	OnEachMeshData(TPredicate Predicate)
	{
		for (int32 i = 0; i < MeshDatas.Num(); ++i)
		{
			Predicate(MeshDatas[i]);
		}
	}

	template<typename TPredicate>
	void	OnEachMeshData(TPredicate Predicate) const
	{
		for (int32 i = 0; i < MeshDatas.Num(); ++i)
		{
			Predicate(MeshDatas[i]);
		}
	}

private:

	TArray<FRPRMeshDataPtr>	MeshDatas;

};

typedef TSharedPtr<FRPRMeshDataContainer> FRPRMeshDataContainerPtr;
typedef TWeakPtr<FRPRMeshDataContainer> FRPRMeshDataContainerWkPtr;

#undef DECLARE_BROADCAST
