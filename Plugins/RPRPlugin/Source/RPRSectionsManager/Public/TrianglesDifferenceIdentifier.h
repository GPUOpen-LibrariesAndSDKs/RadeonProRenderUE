#pragma once
#include "AsyncWork.h"
#include "RPRMeshData.h"
#include "TrianglesSelectionFlags.h"

class FTrianglesDifferenceIdentifier
{
public:
	virtual ~FTrianglesDifferenceIdentifier();

	void EnqueueNewTask(
		const FRPRMeshDataPtr MeshDataPtr, 
		FTrianglesSelectionFlags* SelectionFlags,
		TArray<uint32>& NewTriangles, 
		const TArray<uint32>* MeshIndices
	);

	bool IsLastTaskCompleted() const;
	const FRPRMeshDataPtr GetLastTaskRPRMeshData() const;
	const TArray<uint32>& GetLastTaskResult() const;
	void DequeueCompletedTask();
	bool HasTasks() const;
	void AbortAllTasks();

	static TArray<uint32> ExecuteTask(
		const FRPRMeshDataPtr MeshDataPtr,
		FTrianglesSelectionFlags* SelectionFlags,
		TArray<uint32>& NewTriangles,
		const TArray<uint32>* MeshIndices);

private:

	class FTriangleDiffAsyncTask
	{
		friend class FAsyncTask<FTriangleDiffAsyncTask>;

	public:

		FRPRMeshDataPtr MeshDataPtr;
		FTrianglesSelectionFlags* SelectionFlags;
		TArray<uint32> NewTriangles;
		const TArray<uint32>* MeshIndices;
		TArray<uint32> NewIndicesSelected;
		int32 NumTrianglesDone;
		bool bIsTaskCompleted;
		bool bIsCancelled;

		FTriangleDiffAsyncTask()
			: MeshIndices(nullptr)
			, NumTrianglesDone(0)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		FTriangleDiffAsyncTask(FRPRMeshDataPtr meshDataPtr, FTrianglesSelectionFlags* selectionFlags, TArray<uint32>&& newTriangles, const TArray<uint32>* meshIndices)
			: MeshDataPtr(meshDataPtr)
			, SelectionFlags(selectionFlags)
			, NewTriangles(MoveTemp(newTriangles))
			, MeshIndices(meshIndices)
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
