#pragma once
#include "AsyncWork.h"
#include "RPRMeshData.h"

class FTrianglesDifferenceIdentifier
{
public:
	virtual ~FTrianglesDifferenceIdentifier();

	void EnqueueNewTask(const FRPRMeshDataPtr MeshDataPtr, const TArray<uint32>& NewTriangles, const TArray<uint32>* MeshIndices, TArray<uint32>* InOutRegisteredTriangles);
	bool IsLastTaskCompleted() const;
	const FRPRMeshDataPtr GetLastTaskRPRMeshData() const;
	const TArray<uint32>& GetLastTaskResult() const;
	void DequeueCompletedTask();
	bool HasTasks() const;
	void AbortAllTasks();


private:

	class FTriangleDiffAsyncTask
	{
		friend class FAsyncTask<FTriangleDiffAsyncTask>;

	public:

		FRPRMeshDataPtr MeshDataPtr;
		const TArray<uint32>* NewTriangles;
		const TArray<uint32>* MeshIndices;
		TArray<uint32>* RegisteredTriangles;
		TArray<uint32> NewIndicesSelected;
		int32 TriangleOffset;
		int32 NumTrianglesDone;
		bool bIsInitialized;
		bool bIsTaskCompleted;
		bool bIsCancelled;

		FTriangleDiffAsyncTask()
			: NewTriangles(nullptr)
			, MeshIndices(nullptr)
			, TriangleOffset(0)
			, NumTrianglesDone(0)
			, bIsInitialized(false)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		FTriangleDiffAsyncTask(FRPRMeshDataPtr meshDataPtr, const TArray<uint32>& newTriangles, const TArray<uint32>* meshIndices, TArray<uint32>* registeredTriangles)
			: MeshDataPtr(meshDataPtr)
			, NewTriangles(new TArray<uint32>(newTriangles))
			, MeshIndices(meshIndices)
			, RegisteredTriangles(registeredTriangles)
			, TriangleOffset(0)
			, NumTrianglesDone(0)
			, bIsInitialized(false)
			, bIsTaskCompleted(false)
			, bIsCancelled(false)
		{}

		~FTriangleDiffAsyncTask()
		{
			delete NewTriangles;
		}

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
