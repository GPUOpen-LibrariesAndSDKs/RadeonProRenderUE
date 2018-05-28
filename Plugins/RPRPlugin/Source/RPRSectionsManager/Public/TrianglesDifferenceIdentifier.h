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
