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
#include "RPRMeshData.h"
#include "StaticMeshHelper.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "UVUtility.h"
#include "RPRConstAway.h"

#include "FileHelper.h"
#include "PlatformFilemanager.h"

DECLARE_LOG_CATEGORY_CLASS(RPRMeshDataLog, Log, All)

DECLARE_CYCLE_STAT(TEXT("RPRMeshData ~ Apply Raw Mesh Datas"), STAT_ApplyRawMeshDatas, STATGROUP_RPRMeshData)
DECLARE_CYCLE_STAT(TEXT("RPRMeshData ~ Dump UV"), STAT_DumpUV, STATGROUP_RPRMeshData)
DECLARE_CYCLE_STAT(TEXT("RPRMeshData ~ Update All Barycenters"), STAT_UpdateAllBarycenters, STATGROUP_RPRMeshData)
DECLARE_CYCLE_STAT(TEXT("RPRMeshData ~ Update Barycenter"), STAT_UpdateBarycenter, STATGROUP_RPRMeshData)

FRPRMeshData::FRPRMeshData(UStaticMesh* InStaticMesh)
	: StaticMesh(InStaticMesh)
	, bHasMeshChangesNotCommitted(false)
{
	const int32 lodIndex = 0;
	Sections.AddDefaulted(StaticMesh->GetNumSections(lodIndex));

	FStaticMeshHelper::LoadRawMeshFromStaticMesh(InStaticMesh, RawMesh);

	Barycenters.AddDefaulted(MAX_MESH_TEXTURE_COORDS);
	UpdateAllBarycenters();
}

void FRPRMeshData::AssignPreview(URPRStaticMeshPreviewComponent* InPreviewMeshComponent)
{
	Preview = InPreviewMeshComponent;
}

void FRPRMeshData::ApplyRawMeshDatas()
{
	SCOPE_CYCLE_COUNTER(STAT_ApplyRawMeshDatas);

	if (StaticMesh.IsValid() && RawMesh.IsValid())
	{
		FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh.Get());
		bHasMeshChangesNotCommitted = false;

		NotifyStaticMeshChanges();

		StaticMesh->MarkPackageDirty();
		if (Preview.IsValid())
		{
			Preview->MarkRenderStateDirty();
		}
	}
}

void FRPRMeshData::NotifyRawMeshChanges()
{
	bHasMeshChangesNotCommitted = true;
	UpdateAllBarycenters();
	OnPostRawMeshChange.Broadcast();
}

void FRPRMeshData::NotifyStaticMeshChanges()
{
	OnPostStaticMeshChange.Broadcast();
}

void FRPRMeshData::NotifyStaticMeshMaterialChanges()
{
	OnPostStaticMeshMaterialChange.Broadcast();
}

int32 FRPRMeshData::GetNumUVChannelsUsed() const
{
	int32 numChannels = 0;

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		bool bIsUVUsed = (RawMesh.WedgeTexCoords[i].Num() > 0);
		if (!bIsUVUsed)
		{
			break;
		}
		++numChannels;
	}

	return (numChannels);
}

const FVector2D& FRPRMeshData::GetUVBarycenter(int32 UVChannel) const
{
	return (Barycenters[UVChannel]);
}

bool FRPRMeshData::HasMeshChangesNotCommitted() const
{
	return (bHasMeshChangesNotCommitted);
}

void FRPRMeshData::DumpUV(int32 UVChannel)
{
	SCOPE_CYCLE_COUNTER(STAT_DumpUV);

	if (RawMesh.IsValid())
	{
		const FRawMesh& rawMesh = GetRawMesh();
		const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[UVChannel];

		TArray<FString> Lines;

		UE_LOG(RPRMeshDataLog, Log, TEXT("== Dump UV : %s (%d UV) =="), *GetStaticMesh()->GetName(), uv.Num());

		for (int32 uvIndex = 0; uvIndex < uv.Num(); ++uvIndex)
		{
			Lines.Add(FString::Printf(TEXT("UV[%d] = %s"), uvIndex, *uv[uvIndex].ToString()));
		}

		FString filename = FString::Printf(TEXT("DumpUV_%s.txt"), *GetStaticMesh()->GetName());
		FString path = FPaths::Combine(FPaths::ProjectSavedDir(), filename);
		if (FPaths::FileExists(path))
		{
			IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
			platformFile.DeleteFile(*path);
		}
		FFileHelper::SaveStringArrayToFile(Lines, *path);
	}
}

void FRPRMeshData::UpdateAllBarycenters()
{
	SCOPE_CYCLE_COUNTER(STAT_UpdateAllBarycenters);

	int32 numUVChannelsUsed = GetNumUVChannelsUsed();

	for (int32 uvChannelIndex = 0; uvChannelIndex < numUVChannelsUsed; ++uvChannelIndex)
	{
		UpdateBarycenter(uvChannelIndex);
	}
}

void FRPRMeshData::UpdateBarycenter(int32 UVChannel)
{
	SCOPE_CYCLE_COUNTER(STAT_UpdateBarycenter);

	FVector2D& barycenter = Barycenters[UVChannel];
	barycenter = FVector2D(EForceInit::ForceInitToZero);

	if (RawMesh.IsValid())
	{
		const FRawMesh& rawMesh = GetRawMesh();
		const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[UVChannel];

		if (uv.Num() > 0)
		{
			// Use weight instead of dividing everything at end
			// so it can handle large quantity of UV without overflowing
			// - the weight can be a bad idea if there is too much invalid UV
			// but... come on, it shouldn't happen
			const float weight = 1.0f / uv.Num();
			for (int32 uvIndex = 0; uvIndex < uv.Num(); ++uvIndex)
			{
				if (FUVUtility::IsUVValid(uv[uvIndex]))
				{
					barycenter += uv[uvIndex] * weight;
				}
			}
		}
	}
}

void FRPRMeshData::UpdatePreviewSectionHighlights()
{
	if (Preview.IsValid())
	{
		Preview->ClearSectionSelection();
		TArray<int32> highlightedSectionIndexes;
		for (int32 sectionIndex = 0; sectionIndex < Sections.Num(); ++sectionIndex)
		{
			if (Sections[sectionIndex].IsHighlighted())
			{
				highlightedSectionIndexes.Add(sectionIndex);
			}
		}
		Preview->SelectSections(highlightedSectionIndexes);
	}
}

void FRPRMeshData::RebuildSections()
{
	if (StaticMesh.IsValid())
	{
		const int32 lodIndex = 0;
		int32 newNumSections = StaticMesh->GetNumSections(lodIndex);
		if (newNumSections > Sections.Num())
		{
			Sections.AddDefaulted(newNumSections - Sections.Num());
		}
		else if (newNumSections < Sections.Num())
		{
			Sections.RemoveAt(newNumSections - 1, Sections.Num() - newNumSections);
		}
	}
}

void FRPRMeshData::HighlightSection(int32 SectionIndex, bool bHighlight)
{
	GetMeshSection(SectionIndex).Highlight(bHighlight);
	UpdatePreviewSectionHighlights();
}

FRPRMeshSection& FRPRMeshData::GetMeshSection(int32 Index)
{
	const FRPRMeshData* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetMeshSection(Index)));
}

const FRPRMeshSection& FRPRMeshData::GetMeshSection(int32 Index) const
{
	return (Sections[Index]);
}

int32 FRPRMeshData::GetNumSections() const
{
	const int32 lodIndex = 0;
	return (StaticMesh.IsValid() ? StaticMesh->GetNumSections(lodIndex) : 0);
}

int32 FRPRMeshData::CountNumSelectedSections() const
{
	int32 counter = 0;
	for (int32 i = 0; i < Sections.Num(); ++i)
	{
		if (Sections[i].IsSelected())
		{
			++counter;
		}
	}
	return (counter);
}

bool FRPRMeshData::HasAtLeastOneSectionSelected() const
{
	for (int32 i = 0; i < Sections.Num(); ++i)
	{
		if (Sections[i].IsSelected())
		{
			return (true);
		}
	}
	return (false);
}

int32 FRPRMeshData::FindFirstSelectedSectionIndex() const
{
	for (int32 sectionIndex = 0; sectionIndex < Sections.Num(); ++sectionIndex)
	{
		if (Sections[sectionIndex].IsSelected())
		{
			return (sectionIndex);
		}
	}
	return (INDEX_NONE);
}
