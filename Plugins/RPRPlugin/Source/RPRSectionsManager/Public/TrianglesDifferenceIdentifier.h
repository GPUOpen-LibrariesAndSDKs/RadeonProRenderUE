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
#include "AsyncWork.h"
#include "RPRMeshData.h"
#include "TrianglesSelectionFlags.h"
#include "DynamicSelectionMeshVisualizer.h"

class FTrianglesDifferenceIdentifier
{
public:
	virtual ~FTrianglesDifferenceIdentifier();

	void EnqueueAsyncSelection(
		const FRPRMeshDataPtr MeshDataPtr, 
		FTrianglesSelectionFlags* SelectionFlags,
		UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
		TArray<uint32>& NewTriangles
	);

	void EnqueueAsyncDeselection(
		const FRPRMeshDataPtr MeshDataPtr,
		FTrianglesSelectionFlags* SelectionFlags,
		UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
		TArray<uint32>& Triangles
	);

	bool IsLastTaskCompleted() const;
	const FRPRMeshDataPtr GetLastTaskRPRMeshData() const;
	void DequeueCompletedTask();
	bool HasTasks() const;
	void AbortAllTasks();

	static void SelectNewTriangles(
		const FRPRMeshDataPtr MeshDataPtr,
		FTrianglesSelectionFlags* SelectionFlags,
		UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
		TArray<uint32>& NewTriangles, bool bSelect = true);

private:

	class FTriangleDiffAsyncTask
	{
		friend class FAsyncTask<FTriangleDiffAsyncTask>;

	public:

		FRPRMeshDataPtr MeshDataPtr;
		FTrianglesSelectionFlags* SelectionFlags;
		TArray<uint32> NewTriangles;
		UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer;
		int32 NumTrianglesDone;
		bool bIsTaskCompleted;
		bool bIsSelecting;
		bool bIsCancelled;

		FTriangleDiffAsyncTask()
			: SelectionFlags(nullptr)
			, SelectionVisualizer(nullptr)
			, NumTrianglesDone(0)
			, bIsTaskCompleted(false)
			, bIsSelecting(false)
			, bIsCancelled(false)
		{}

		FTriangleDiffAsyncTask(FRPRMeshDataPtr meshDataPtr, FTrianglesSelectionFlags* selectionFlags, UDynamicSelectionMeshVisualizerComponent* selectionVisualizer, TArray<uint32>&& newTriangles, bool IsSelecting)
			: MeshDataPtr(meshDataPtr)
			, SelectionFlags(selectionFlags)
			, NewTriangles(MoveTemp(newTriangles))
			, SelectionVisualizer(selectionVisualizer)
			, NumTrianglesDone(0)
			, bIsTaskCompleted(false)
			, bIsSelecting(IsSelecting)
			, bIsCancelled(false)
		{}

		void DoWork();
		void Abandon();
		
		FORCEINLINE bool CanAbandon() const { return (true); }
		FORCEINLINE TStatId GetStatId() const 
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(TriangleDiffAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
		}

	private:
		void SelectTriangle(uint32 TriangleValue);
		void DeselectTriangle(uint32 TriangleValue);

	};

private:

	void EnqueueTaskAndStartIfRequired(FAsyncTask<FTriangleDiffAsyncTask>* Task);

private:

	TQueue<FAsyncTask<FTriangleDiffAsyncTask>*> Tasks;
};
