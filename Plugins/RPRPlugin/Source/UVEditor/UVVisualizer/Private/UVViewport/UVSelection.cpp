#include "UVSelection.h"
#include "UVViewportClient.h"

FUVSelection::FUVSelection()
	: CachedUVs(nullptr)
{}

void FUVSelection::SetCachedUVs(TArray<FUVVertexData>* InCachedUVs)
{
	CachedUVs = InCachedUVs;
}

void FUVSelection::SelectUV(int32 UVIndex)
{
	ClearSelection();
	AddUV(UVIndex);
}

void FUVSelection::AddUV(int32 UVIndex)
{
	SelectedUVs.Add(UVIndex);
	if (CachedUVs != nullptr)
	{
		GetUV(UVIndex).bIsSelected = true;
	}
}

void FUVSelection::DeselectUV(int32 UVIndex)
{
	SelectedUVs.Remove(UVIndex);
	if (CachedUVs != nullptr)
	{
		GetUV(UVIndex).bIsSelected = false;
	}
}

bool FUVSelection::IsUVSelected(int32 UVIndex) const
{
	return (CachedUVs != nullptr ? GetUV(UVIndex).bIsSelected : SelectedUVs.Contains(UVIndex));
}

const TArray<int32>& FUVSelection::GetSelectedUV() const
{
	return (SelectedUVs);
}

void FUVSelection::ClearSelection()
{
	SelectedUVs.Empty();
	if (CachedUVs != nullptr)
	{
		for (int32 i = 0; i < CachedUVs->Num(); ++i)
		{
			GetUV(i).bIsSelected = false;
		}
	}
}

FUVVertexData& FUVSelection::GetUV(int32 UVIndex)
{
	return ((*CachedUVs)[UVIndex]);
}

const FUVVertexData& FUVSelection::GetUV(int32 UVIndex) const
{
	return ((*CachedUVs)[UVIndex]);
}
