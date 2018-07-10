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
#include "RPRSelectionManager.h"
#include "Helpers/RPRConstAway.h"

TSharedPtr<FRPRSectionsSelectionManager> FRPRSectionsSelectionManager::Instance;

void FRPRSectionsSelectionManager::Register()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FRPRSectionsSelectionManager);
	}
}

void FRPRSectionsSelectionManager::Unregister()
{
	Instance.Reset();
}

FRPRSectionsSelectionManager& FRPRSectionsSelectionManager::Get()
{
	return (*Instance.Get());
}

void FRPRSectionsSelectionManager::SetSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	if (!SelectedTrianglesMap.Contains(MeshData))
	{
		RegisterNewMeshEntry(MeshData);
	}

	FTrianglesSelectionFlags& triangleSelectionFlags = SelectedTrianglesMap[MeshData];
	triangleSelectionFlags.ResetAllFlags();
	triangleSelectionFlags.SetFlagAsUsed(Triangles);

	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::AppendSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	if (!SelectedTrianglesMap.Contains(MeshData))
	{
		RegisterNewMeshEntry(MeshData);
	}

	FTrianglesSelectionFlags& triangleSelectionFlags = SelectedTrianglesMap[MeshData];
	triangleSelectionFlags.SetFlagAsUsed(Triangles);

	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::RemoveFromSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	FTrianglesSelectionFlags* selectedTriangles = SelectedTrianglesMap.Find(MeshData);
	if (selectedTriangles != nullptr)
	{
		selectedTriangles->SetFlagAsUnused(Triangles);
		OnSelectionChangedEvent.Broadcast();
	}
}

void FRPRSectionsSelectionManager::RegisterNewMeshEntry(const FRPRMeshDataPtr MeshDataPtr)
{
	const int32 numMaxTriangles = MeshDataPtr->GetStaticMesh()->RenderData->LODResources[0].IndexBuffer.GetNumIndices() / 3;
	SelectedTrianglesMap.Add(MeshDataPtr, FTrianglesSelectionFlags(numMaxTriangles));
}

void FRPRSectionsSelectionManager::ClearSelectionFor(const FRPRMeshDataPtr MeshData)
{
	SelectedTrianglesMap.Remove(MeshData);
	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::ClearAllSelection()
{
	SelectedTrianglesMap.Empty();
	OnSelectionChangedEvent.Broadcast();
}

bool FRPRSectionsSelectionManager::HasSelectedTriangles(const FRPRMeshDataPtr MeshData)
{
	auto result = GetTriangleSelection(MeshData);
	return (result != nullptr && result->HasAtTrianglesSelected());
}

bool FRPRSectionsSelectionManager::IsTriangleSelected(const FRPRMeshDataPtr MeshData, uint32 Triangle) const
{
	return (GetTriangleSelection(MeshData)->IsTriangleUsed(Triangle));
}

FTrianglesSelectionFlags* FRPRSectionsSelectionManager::CreateOrGetTriangleSelection(const FRPRMeshDataPtr MeshData)
{
	FTrianglesSelectionFlags* selectionFlags = GetTriangleSelection(MeshData);
	if (selectionFlags == nullptr)
	{
		RegisterNewMeshEntry(MeshData);
		return (GetTriangleSelection(MeshData));
	}
	return (selectionFlags);
}

const FTrianglesSelectionFlags* FRPRSectionsSelectionManager::GetTriangleSelection(const FRPRMeshDataPtr MeshData) const
{
	return (SelectedTrianglesMap.Find(MeshData));
}

FTrianglesSelectionFlags* FRPRSectionsSelectionManager::GetTriangleSelection(const FRPRMeshDataPtr MeshData)
{
	const FRPRSectionsSelectionManager* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetTriangleSelection(MeshData)));
}

FRPRSectionsSelectionManager::FSelectionMap::TIterator FRPRSectionsSelectionManager::GetSelectionIterator()
{
	return (SelectedTrianglesMap.CreateIterator());
}

const FRPRMeshDataPtr FRPRSectionsSelectionManager::FindMeshDataByStaticMesh(UStaticMesh* StaticMesh) const
{
	for (auto it = SelectedTrianglesMap.CreateConstIterator(); it; ++it)
	{
		const FRPRMeshDataPtr meshData = it.Key();
		if (meshData->GetStaticMesh() == StaticMesh)
		{
			return (meshData);
		}
	}
	return (nullptr);
}
