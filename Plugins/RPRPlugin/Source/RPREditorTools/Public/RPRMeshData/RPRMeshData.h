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
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
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
	int32	CountNumSelectedSections() const;
	bool	HasAtLeastOneSectionSelected() const;
	int32	FindFirstSelectedSectionIndex() const;

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
