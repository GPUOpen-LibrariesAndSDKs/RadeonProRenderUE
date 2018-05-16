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

	bool IsLastTaskCompleted() const;
	const FRPRMeshDataPtr GetLastTaskRPRMeshData() const;
	void DequeueCompletedTask();
	bool HasTasks() const;
	void AbortAllTasks();

	static void ExecuteTask(
		const FRPRMeshDataPtr MeshDataPtr,
		FTrianglesSelectionFlags* SelectionFlags,
		UDynamicSelectionMeshVisualizerComponent* SelectionVisualizer,
		TArray<uint32>& NewTriangles);

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
		bool bIsCancelled;

		FTriangleDiffAsyncTask()
			: SelectionFlags(nullptr)
			, SelectionVisualizer(nullptr)
			, NumTrianglesDone(0)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		FTriangleDiffAsyncTask(FRPRMeshDataPtr meshDataPtr, FTrianglesSelectionFlags* selectionFlags, UDynamicSelectionMeshVisualizerComponent* selectionVisualizer, TArray<uint32>&& newTriangles)
			: MeshDataPtr(meshDataPtr)
			, SelectionFlags(selectionFlags)
			, NewTriangles(MoveTemp(newTriangles))
			, SelectionVisualizer(selectionVisualizer)
			, NumTrianglesDone(0)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		void DoWork();
		void Abandon();
		
		FORCEINLINE bool CanAbandon() const { return (true); }
		FORCEINLINE TStatId GetStatId() const 
		{
			RETURN_QUICK_DECLARE_CYCLE_STAT(TriangleDiffAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
		}

	};

	TQueue<FAsyncTask<FTriangleDiffAsyncTask>*> Tasks;
};
